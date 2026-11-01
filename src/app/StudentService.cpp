#include "lms/app/Services.hpp"
#include <algorithm>
#include <iterator>

namespace lms {

StudentService::StudentService(IAssignmentRepository& assignments, ISubmissionRepository& submissions,
                               IExtensionRepository& extensions, ICourseRepository& courses,
                               LatePenaltyStrategy& penalty)
    : assignments_(assignments), submissions_(submissions), extensions_(extensions),
      courses_(courses), penalty_(penalty) {}

std::vector<Assignment> StudentService::listAssignments(int studentId) {
    return assignments_.findForStudent(studentId);
}

std::vector<StudentAssignmentView> StudentService::listAssignmentsWithDeadlines(int studentId) {
    const auto assignments = assignments_.findForStudent(studentId);
    std::vector<StudentAssignmentView> views;
    views.reserve(assignments.size());
    std::transform(assignments.begin(), assignments.end(), std::back_inserter(views),
                   [this, studentId](const Assignment& assignment) {
                       return StudentAssignmentView{
                           assignment, effectiveDeadline(assignment, studentId)};
                   });
    return views;
}

std::string StudentService::effectiveDeadline(const Assignment& assignment, int studentId) {
    std::string deadline = assignment.deadline;
    if (const auto extension = extensions_.latestFor(assignment.id, studentId)) {
        if (extension->newDeadline > deadline) {
            deadline = extension->newDeadline;
        }
    }
    return deadline;
}

std::vector<Submission> StudentService::listReleasedSubmissions(int studentId) {
    return submissions_.findByStudentAndStatus(studentId, SubmissionStatus::Released);
}

void StudentService::submit(int studentId, int assignmentId, const std::string& content) {
    if (content.empty()) throw ValidationError("Submission content cannot be empty");

    auto assignment = assignments_.findById(assignmentId);
    if (!assignment) throw ValidationError("Assignment not found");

    if (!courses_.isEnrolled(studentId, assignment->courseId))
        throw ValidationError("You are not enrolled in this course");

    const std::string submittedAt = nowIso();
    const int late = daysLate(effectiveDeadline(*assignment, studentId), submittedAt);

    Submission s;
    s.assignmentId = assignmentId;
    s.studentId = studentId;
    s.content = content;
    s.submittedAt = submittedAt;
    s.daysLate = late;
    s.status = (late > 0) ? SubmissionStatus::Late : SubmissionStatus::Submitted;

    // Warn on resubmission is handled by UI; repository replaces latest version
    if (auto existing = submissions_.findFor(assignmentId, studentId)) {
        if (existing->status != SubmissionStatus::Submitted &&
            existing->status != SubmissionStatus::Late) {
            throw ValidationError("Cannot resubmit after marking has started");
        }
    }

    submissions_.save(s);
}

} // namespace lms
