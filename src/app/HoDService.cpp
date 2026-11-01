#include "lms/app/Services.hpp"

namespace lms {

HoDService::HoDService(ISubmissionRepository& submissions, IAuditRepository& audit)
    : submissions_(submissions), audit_(audit) {}

std::vector<Submission> HoDService::markedInDepartment(int departmentId) {
    return submissions_.findByDepartmentAndStatus(departmentId, SubmissionStatus::Marked);
}

std::vector<Submission> HoDService::releasedInDepartment(int departmentId) {
    return submissions_.findByDepartmentAndStatus(departmentId, SubmissionStatus::Released);
}

void HoDService::moderate(int submissionId, std::optional<double> newMark,
                          const std::string& reason, int hodId) {
    auto sub = submissions_.findById(submissionId);
    if (!sub) throw ValidationError("Submission not found");
    if (sub->status != SubmissionStatus::Marked && sub->status != SubmissionStatus::Moderated)
        throw ValidationError("Submission is not ready for moderation");

    if (newMark) {
        if (reason.empty()) throw ValidationError("A reason is required when changing a mark");
        const double oldVal = sub->finalMark.value_or(sub->rawMark.value_or(0.0));
        submissions_.updateMarkAndStatus(submissionId, sub->rawMark.value_or(*newMark),
                                         *newMark, sub->feedback.value_or(""),
                                         SubmissionStatus::Moderated);
        AuditEntry entry;
        entry.submissionId = submissionId;
        entry.changedBy = hodId;
        entry.changedAt = nowIso();
        entry.oldMark = oldVal;
        entry.newMark = *newMark;
        entry.reason = reason;
        audit_.append(entry);
    } else {
        submissions_.setStatus(submissionId, SubmissionStatus::Moderated);
    }
}

void HoDService::release(int submissionId) {
    auto sub = submissions_.findById(submissionId);
    if (!sub) throw ValidationError("Submission not found");
    if (sub->status != SubmissionStatus::Marked && sub->status != SubmissionStatus::Moderated)
        throw ValidationError("Only marked or moderated submissions can be released");
    submissions_.setStatus(submissionId, SubmissionStatus::Released);
}

} // namespace lms
