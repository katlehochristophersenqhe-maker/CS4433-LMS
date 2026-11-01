#include "lms/app/Services.hpp"

namespace lms {

LecturerService::LecturerService(IAssignmentRepository& assignments, ISubmissionRepository& submissions,
                                 IExtensionRepository& extensions, IAuditRepository& audit,
                                 LatePenaltyStrategy& penalty)
    : assignments_(assignments), submissions_(submissions), extensions_(extensions),
      audit_(audit), penalty_(penalty) {}

int LecturerService::createAssignment(int courseId, const std::string& title,
                                      const std::string& description,
                                      const std::string& deadline, double maxMark) {
    if (title.empty()) throw ValidationError("Title is required");
    if (maxMark <= 0) throw ValidationError("Maximum mark must be positive");
    Assignment a;
    a.courseId = courseId;
    a.title = title;
    a.description = description;
    a.deadline = deadline;
    a.maxMark = maxMark;
    return assignments_.save(a);
}

void LecturerService::grantExtension(int assignmentId, int studentId,
                                     const std::string& newDeadline, const std::string& reason,
                                     int lecturerId) {
    saveExtension(assignmentId, studentId, newDeadline, reason, lecturerId);
}

void LecturerService::grantClassExtension(int assignmentId, const std::string& newDeadline,
                                          const std::string& reason, int lecturerId) {
    saveExtension(assignmentId, std::nullopt, newDeadline, reason, lecturerId);
}

void LecturerService::saveExtension(int assignmentId, std::optional<int> studentId,
                                    const std::string& newDeadline, const std::string& reason,
                                    int lecturerId) {
    if (reason.empty()) throw ValidationError("Extension reason is required");
    auto assignment = assignments_.findById(assignmentId);
    if (!assignment) throw ValidationError("Assignment not found");
    if (newDeadline <= assignment->deadline)
        throw ValidationError("Extension must be later than the assignment deadline");
    Extension e;
    e.assignmentId = assignmentId;
    e.studentId = studentId;
    e.newDeadline = newDeadline;
    e.reason = reason;
    e.grantedBy = lecturerId;
    extensions_.add(e);
}

void LecturerService::markSubmission(int submissionId, double rawMark,
                                     const std::string& feedback, int lecturerId) {
    if (feedback.empty()) throw ValidationError("Written feedback is required");
    auto sub = submissions_.findById(submissionId);
    if (!sub) throw ValidationError("Submission not found");
    if (sub->status != SubmissionStatus::Submitted && sub->status != SubmissionStatus::Late)
        throw ValidationError("Submission is not ready for marking");

    auto assignment = assignments_.findById(sub->assignmentId);
    if (!assignment) throw PersistenceError("Assignment missing for submission");
    if (rawMark < 0 || rawMark > assignment->maxMark)
        throw ValidationError("Mark must be between 0 and " + std::to_string(assignment->maxMark));

    const double finalMark = penalty_.apply(assignment->maxMark, rawMark, sub->daysLate);

    // Transactional intent: mark + audit
    submissions_.updateMarkAndStatus(submissionId, rawMark, finalMark, feedback,
                                     SubmissionStatus::Marked);
    AuditEntry entry;
    entry.submissionId = submissionId;
    entry.changedBy = lecturerId;
    entry.changedAt = nowIso();
    entry.oldMark = std::nullopt;
    entry.newMark = finalMark;
    entry.reason = "Initial mark by lecturer";
    audit_.append(entry);
}

std::vector<Submission> LecturerService::listSubmissions(int assignmentId) {
    // Return all statuses for this assignment – UI can filter
    std::vector<Submission> all;
    for (auto status : {SubmissionStatus::Submitted, SubmissionStatus::Late,
                        SubmissionStatus::Marked, SubmissionStatus::Moderated,
                        SubmissionStatus::Released}) {
        auto part = submissions_.findByAssignmentAndStatus(assignmentId, status);
        all.insert(all.end(), part.begin(), part.end());
    }
    return all;
}

} // namespace lms
