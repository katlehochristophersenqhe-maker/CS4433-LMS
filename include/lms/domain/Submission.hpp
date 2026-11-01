#pragma once
#include "lms/domain/Types.hpp"
#include <optional>
#include <string>

namespace lms {

struct Submission {
    int id = 0;
    int assignmentId = 0;
    int studentId = 0;
    std::string content;
    std::string submittedAt;          // ISO-8601 UTC text
    SubmissionStatus status = SubmissionStatus::Submitted;
    std::optional<double> rawMark;
    std::optional<std::string> feedback;
    std::optional<double> finalMark;
    int daysLate = 0;

    bool isVisibleToStudent() const {
        return status == SubmissionStatus::Released;
    }
};

struct SubmissionDetails {
    Submission submission;
    std::string assignmentTitle;
    std::string courseCode;
    std::string studentName;
    std::string studentUsername;
};

struct Assignment {
    int id = 0;
    int courseId = 0;
    std::string title;
    std::string description;
    std::string deadline;             // ISO-8601
    double maxMark = 100.0;
};

struct Extension {
    int id = 0;
    int assignmentId = 0;
    std::optional<int> studentId;     // nullopt = class-wide
    std::string newDeadline;
    std::string reason;
    int grantedBy = 0;
};

struct AuditEntry {
    int id = 0;
    int submissionId = 0;
    int changedBy = 0;
    std::string changedAt;
    std::optional<double> oldMark;
    double newMark = 0.0;
    std::string reason;
};

struct Course {
    int id = 0;
    int departmentId = 0;
    std::string code;
    std::string title;
    std::optional<int> lecturerId;
};

struct Department {
    int id = 0;
    std::string name;
};

} // namespace lms
