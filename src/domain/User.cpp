#include "lms/domain/User.hpp"

namespace lms {

User::User(int id, std::string username, std::string fullName, bool active)
    : id_(id), username_(std::move(username)), fullName_(std::move(fullName)), active_(active) {}

Student::Student(int id, std::string username, std::string fullName,
                 std::string studentNumber, bool active)
    : User(id, std::move(username), std::move(fullName), active),
      studentNumber_(std::move(studentNumber)) {}

std::unique_ptr<User> Student::clone() const {
    return std::make_unique<Student>(id_, username_, fullName_, studentNumber_, active_);
}

Lecturer::Lecturer(int id, std::string username, std::string fullName,
                   int departmentId, bool active)
    : User(id, std::move(username), std::move(fullName), active), departmentId_(departmentId) {}

std::unique_ptr<User> Lecturer::clone() const {
    return std::make_unique<Lecturer>(id_, username_, fullName_, departmentId_, active_);
}

HeadOfDepartment::HeadOfDepartment(int id, std::string username, std::string fullName,
                                   int departmentId, bool active)
    : User(id, std::move(username), std::move(fullName), active), departmentId_(departmentId) {}

std::unique_ptr<User> HeadOfDepartment::clone() const {
    return std::make_unique<HeadOfDepartment>(id_, username_, fullName_, departmentId_, active_);
}

Administrator::Administrator(int id, std::string username, std::string fullName, bool active)
    : User(id, std::move(username), std::move(fullName), active) {}

std::unique_ptr<User> Administrator::clone() const {
    return std::make_unique<Administrator>(id_, username_, fullName_, active_);
}

std::unique_ptr<User> UserFactory::create(Role role, int id, const std::string& username,
                                          const std::string& fullName, int departmentId,
                                          const std::string& studentNumber, bool active) {
    switch (role) {
        case Role::Student:
            return std::make_unique<Student>(id, username, fullName, studentNumber, active);
        case Role::Lecturer:
            return std::make_unique<Lecturer>(id, username, fullName, departmentId, active);
        case Role::HoD:
            return std::make_unique<HeadOfDepartment>(id, username, fullName, departmentId, active);
        case Role::Administrator:
            return std::make_unique<Administrator>(id, username, fullName, active);
    }
    throw std::invalid_argument("Unknown role in UserFactory");
}

} // namespace lms
