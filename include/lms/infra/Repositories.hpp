#pragma once
#include "lms/domain/User.hpp"
#include "lms/domain/Submission.hpp"
#include "lms/infra/SqliteDatabase.hpp"
#include "lms/infra/PasswordHasher.hpp"
#include <memory>
#include <optional>
#include <vector>
#include <string>

namespace lms {

class IUserRepository {
public:
    virtual ~IUserRepository() = default;
    virtual std::unique_ptr<User> findById(int id) = 0;
    virtual std::unique_ptr<User> findByUsername(const std::string& username) = 0;
    virtual std::string passwordHash(int userId) = 0;
    virtual int save(const std::string& username, const std::string& hash,
                     const std::string& fullName, Role role,
                     int departmentId, const std::string& studentNumber) = 0;
};

class SqliteUserRepository : public IUserRepository {
public:
    explicit SqliteUserRepository(SqliteDatabase& db);
    std::unique_ptr<User> findById(int id) override;
    std::unique_ptr<User> findByUsername(const std::string& username) override;
    std::string passwordHash(int userId) override;
    int save(const std::string& username, const std::string& hash,
             const std::string& fullName, Role role,
             int departmentId, const std::string& studentNumber) override;
private:
    SqliteDatabase& db_;
    std::unique_ptr<User> mapRow(SqliteStatement& st);
};

class ISubmissionRepository {
public:
    virtual ~ISubmissionRepository() = default;
    virtual std::optional<Submission> findById(int id) = 0;
    virtual std::optional<Submission> findFor(int assignmentId, int studentId) = 0;
    virtual std::vector<Submission> findByAssignmentAndStatus(int assignmentId, SubmissionStatus status) = 0;
    virtual std::vector<Submission> findByDepartmentAndStatus(int departmentId, SubmissionStatus status) = 0;
    virtual void save(Submission& s) = 0;  // insert or replace; sets s.id
    virtual void updateMarkAndStatus(int id, double rawMark, double finalMark,
                                     const std::string& feedback, SubmissionStatus status) = 0;
    virtual void setStatus(int id, SubmissionStatus status) = 0;
};

class SqliteSubmissionRepository : public ISubmissionRepository {
public:
    explicit SqliteSubmissionRepository(SqliteDatabase& db);
    std::optional<Submission> findById(int id) override;
    std::optional<Submission> findFor(int assignmentId, int studentId) override;
    std::vector<Submission> findByAssignmentAndStatus(int assignmentId, SubmissionStatus status) override;
    std::vector<Submission> findByDepartmentAndStatus(int departmentId, SubmissionStatus status) override;
    void save(Submission& s) override;
    void updateMarkAndStatus(int id, double rawMark, double finalMark,
                             const std::string& feedback, SubmissionStatus status) override;
    void setStatus(int id, SubmissionStatus status) override;
private:
    SqliteDatabase& db_;
    Submission mapRow(SqliteStatement& st);
};

class IAssignmentRepository {
public:
    virtual ~IAssignmentRepository() = default;
    virtual std::optional<Assignment> findById(int id) = 0;
    virtual std::vector<Assignment> findByCourse(int courseId) = 0;
    virtual std::vector<Assignment> findForStudent(int studentId) = 0;
    virtual int save(const Assignment& a) = 0;
};

class SqliteAssignmentRepository : public IAssignmentRepository {
public:
    explicit SqliteAssignmentRepository(SqliteDatabase& db);
    std::optional<Assignment> findById(int id) override;
    std::vector<Assignment> findByCourse(int courseId) override;
    std::vector<Assignment> findForStudent(int studentId) override;
    int save(const Assignment& a) override;
private:
    SqliteDatabase& db_;
};

class IExtensionRepository {
public:
    virtual ~IExtensionRepository() = default;
    virtual std::optional<Extension> latestFor(int assignmentId, int studentId) = 0;
    virtual int add(const Extension& e) = 0;
};

class SqliteExtensionRepository : public IExtensionRepository {
public:
    explicit SqliteExtensionRepository(SqliteDatabase& db);
    std::optional<Extension> latestFor(int assignmentId, int studentId) override;
    int add(const Extension& e) override;
private:
    SqliteDatabase& db_;
};

class IAuditRepository {
public:
    virtual ~IAuditRepository() = default;
    virtual int append(const AuditEntry& e) = 0;
    virtual std::vector<AuditEntry> findBySubmission(int submissionId) = 0;
};

class SqliteAuditRepository : public IAuditRepository {
public:
    explicit SqliteAuditRepository(SqliteDatabase& db);
    int append(const AuditEntry& e) override;
    std::vector<AuditEntry> findBySubmission(int submissionId) override;
private:
    SqliteDatabase& db_;
};

class ICourseRepository {
public:
    virtual ~ICourseRepository() = default;
    virtual std::optional<Course> findById(int id) = 0;
    virtual std::vector<Course> findByDepartment(int departmentId) = 0;
    virtual int save(const Course& c) = 0;
    virtual void enrol(int studentId, int courseId) = 0;
    virtual bool isEnrolled(int studentId, int courseId) = 0;
    virtual int saveDepartment(const std::string& name) = 0;
};

class SqliteCourseRepository : public ICourseRepository {
public:
    explicit SqliteCourseRepository(SqliteDatabase& db);
    std::optional<Course> findById(int id) override;
    std::vector<Course> findByDepartment(int departmentId) override;
    int save(const Course& c) override;
    void enrol(int studentId, int courseId) override;
    bool isEnrolled(int studentId, int courseId) override;
    int saveDepartment(const std::string& name) override;
private:
    SqliteDatabase& db_;
};

} // namespace lms
