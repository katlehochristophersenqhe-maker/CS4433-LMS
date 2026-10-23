#pragma once
#include "lms/domain/Types.hpp"
#include <memory>
#include <string>

namespace lms {

class User {
public:
    User(int id, std::string username, std::string fullName, bool active = true);
    virtual ~User() = default;

    int id() const { return id_; }
    const std::string& username() const { return username_; }
    const std::string& fullName() const { return fullName_; }
    bool isActive() const { return active_; }

    virtual Role getRole() const = 0;
    virtual std::unique_ptr<User> clone() const = 0;

protected:
    int id_;
    std::string username_;
    std::string fullName_;
    bool active_;
};

class Student : public User {
public:
    Student(int id, std::string username, std::string fullName,
            std::string studentNumber, bool active = true);
    Role getRole() const override { return Role::Student; }
    const std::string& studentNumber() const { return studentNumber_; }
    std::unique_ptr<User> clone() const override;

private:
    std::string studentNumber_;
};

class Lecturer : public User {
public:
    Lecturer(int id, std::string username, std::string fullName,
             int departmentId, bool active = true);
    Role getRole() const override { return Role::Lecturer; }
    int departmentId() const { return departmentId_; }
    std::unique_ptr<User> clone() const override;

private:
    int departmentId_;
};

class HeadOfDepartment : public User {
public:
    HeadOfDepartment(int id, std::string username, std::string fullName,
                     int departmentId, bool active = true);
    Role getRole() const override { return Role::HoD; }
    int departmentId() const { return departmentId_; }
    std::unique_ptr<User> clone() const override;

private:
    int departmentId_;
};

class Administrator : public User {
public:
    Administrator(int id, std::string username, std::string fullName, bool active = true);
    Role getRole() const override { return Role::Administrator; }
    std::unique_ptr<User> clone() const override;
};

// Factory pattern – centralises creation of role subclasses
class UserFactory {
public:
    static std::unique_ptr<User> create(Role role, int id, const std::string& username,
                                        const std::string& fullName, int departmentId = 0,
                                        const std::string& studentNumber = "",
                                        bool active = true);
};

} // namespace lms
