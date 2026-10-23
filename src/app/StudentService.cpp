#include "lms/app/Services.hpp"

namespace lms {

StudentService::StudentService(IAssignmentRepository& assignments, ISubmissionRepository& submissions,
                               IExtensionRepository& extensions, ICourseRepository& courses,
                               LatePenaltyStrategy& penalty)
    : assignments_(assignments), submissions_(submissions), extensions_(extensions),
      courses_(courses), penalty_(penalty) {}

std::vector<Assignment> StudentService::listAssignments(int studentId) {
    return assignments_.findForStudent(studentId);
}

void StudentService::submit(int studentId, int assignmentId, const std::string& content) {
    if (content.empty()) throw ValidationError("Submission content cannot be empty");

    auto assignment = assignments_.findById(assignmentId);
    if (!assignment) throw ValidationError("Assignment not found");

    if (!courses_.isEnrolled(studentId, assignment->courseId))
        throw ValidationError("You are not enrolled in this course");

    // Effective deadline
    std::string effective = assignment->deadline;
    if (auto ext = extensions_.latestFor(assignmentId, studentId)) {
        if (ext->newDeadline > effective) effective = ext->newDeadline;
    }

    const std::string submittedAt = nowIso();
    int late = daysLate(effective, submittedAt);

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
