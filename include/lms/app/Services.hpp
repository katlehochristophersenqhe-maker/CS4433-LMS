#pragma once
#include "lms/domain/User.hpp"
#include "lms/domain/Submission.hpp"
#include "lms/domain/LatePenaltyStrategy.hpp"
#include "lms/infra/Repositories.hpp"
#include "lms/infra/PasswordHasher.hpp"
#include <memory>
#include <string>
#include <vector>

namespace lms {

// Helper: current UTC time as ISO-8601 string (simplified local time for offline school use)
std::string nowIso();

// Days late: ceil of whole days after deadline (1 second late = 1 day).
int daysLate(const std::string& deadlineIso, const std::string& submittedIso);

struct StudentAssignmentView {
    Assignment assignment;
    std::string effectiveDeadline;
};

class AuthService {
public:
    AuthService(IUserRepository& users, IPasswordHasher& hasher);
    std::unique_ptr<User> login(const std::string& username, const std::string& password);

private:
    IUserRepository& users_;
    IPasswordHasher& hasher_;
};

class StudentService {
public:
    StudentService(IAssignmentRepository& assignments, ISubmissionRepository& submissions,
                   IExtensionRepository& extensions, ICourseRepository& courses,
                   LatePenaltyStrategy& penalty);
    std::vector<Assignment> listAssignments(int studentId);
    std::vector<StudentAssignmentView> listAssignmentsWithDeadlines(int studentId);
    std::vector<Submission> listReleasedSubmissions(int studentId);
    void submit(int studentId, int assignmentId, const std::string& content);

private:
    std::string effectiveDeadline(const Assignment& assignment, int studentId);

    IAssignmentRepository& assignments_;
    ISubmissionRepository& submissions_;
    IExtensionRepository& extensions_;
    ICourseRepository& courses_;
    LatePenaltyStrategy& penalty_;
};

class LecturerService {
public:
    LecturerService(IAssignmentRepository& assignments, ISubmissionRepository& submissions,
                    IExtensionRepository& extensions, IAuditRepository& audit,
                    LatePenaltyStrategy& penalty);
    int createAssignment(int courseId, const std::string& title, const std::string& description,
                         const std::string& deadline, double maxMark);
    void grantExtension(int assignmentId, int studentId, const std::string& newDeadline,
                        const std::string& reason, int lecturerId);
    void grantClassExtension(int assignmentId, const std::string& newDeadline,
                             const std::string& reason, int lecturerId);
    void markSubmission(int submissionId, double rawMark, const std::string& feedback, int lecturerId);
    std::vector<Submission> listSubmissions(int assignmentId);
    std::vector<SubmissionDetails> listSubmissionsForLecturer(int lecturerId);

private:
    void saveExtension(int assignmentId, std::optional<int> studentId,
                       const std::string& newDeadline, const std::string& reason,
                       int lecturerId);

    IAssignmentRepository& assignments_;
    ISubmissionRepository& submissions_;
    IExtensionRepository& extensions_;
    IAuditRepository& audit_;
    LatePenaltyStrategy& penalty_;
};

class HoDService {
public:
    HoDService(ISubmissionRepository& submissions, IAuditRepository& audit);
    std::vector<Submission> markedInDepartment(int departmentId);
    std::vector<Submission> releasedInDepartment(int departmentId);
    void moderate(int submissionId, std::optional<double> newMark, const std::string& reason, int hodId);
    void release(int submissionId);

private:
    ISubmissionRepository& submissions_;
    IAuditRepository& audit_;
};

class AdminService {
public:
    AdminService(IUserRepository& users, ICourseRepository& courses, IPasswordHasher& hasher);
    int createUser(const std::string& username, const std::string& password,
                   const std::string& fullName, Role role, int departmentId = 0,
                   const std::string& studentNumber = "");
    int createDepartment(const std::string& name);
    int createCourse(int departmentId, const std::string& code, const std::string& title, int lecturerId);
    void enrolStudent(int studentId, int courseId);

private:
    IUserRepository& users_;
    ICourseRepository& courses_;
    IPasswordHasher& hasher_;
};

} // namespace lms
