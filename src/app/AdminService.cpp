#include "lms/app/Services.hpp"

namespace lms {

AdminService::AdminService(IUserRepository& users, ICourseRepository& courses, IPasswordHasher& hasher)
    : users_(users), courses_(courses), hasher_(hasher) {}

int AdminService::createUser(const std::string& username, const std::string& password,
                             const std::string& fullName, Role role, int departmentId,
                             const std::string& studentNumber) {
    if (username.empty() || password.empty() || fullName.empty())
        throw ValidationError("Username, password and full name are required");
    if (users_.findByUsername(username))
        throw ValidationError("Username already exists");
    const std::string hash = hasher_.hash(password);
    return users_.save(username, hash, fullName, role, departmentId, studentNumber);
}

int AdminService::createDepartment(const std::string& name) {
    if (name.empty()) throw ValidationError("Department name is required");
    return courses_.saveDepartment(name);
}

int AdminService::createCourse(int departmentId, const std::string& code,
                               const std::string& title, int lecturerId) {
    if (code.empty() || title.empty()) throw ValidationError("Code and title are required");
    Course c;
    c.departmentId = departmentId;
    c.code = code;
    c.title = title;
    if (lecturerId > 0) c.lecturerId = lecturerId;
    return courses_.save(c);
}

void AdminService::enrolStudent(int studentId, int courseId) {
    courses_.enrol(studentId, courseId);
}

} // namespace lms
