#pragma once
#include <string>
#include <stdexcept>

namespace lms {

enum class Role {
    Student,
    Lecturer,
    HoD,
    Administrator
};

inline std::string toString(Role r) {
    switch (r) {
        case Role::Student:        return "Student";
        case Role::Lecturer:       return "Lecturer";
        case Role::HoD:            return "HoD";
        case Role::Administrator:  return "Administrator";
    }
    return "Unknown";
}

inline Role roleFromString(const std::string& s) {
    if (s == "Student")       return Role::Student;
    if (s == "Lecturer")      return Role::Lecturer;
    if (s == "HoD")           return Role::HoD;
    if (s == "Administrator") return Role::Administrator;
    throw std::invalid_argument("Unknown role: " + s);
}

enum class SubmissionStatus {
    Submitted,
    Late,
    Marked,
    Moderated,
    Released
};

inline std::string toString(SubmissionStatus s) {
    switch (s) {
        case SubmissionStatus::Submitted: return "Submitted";
        case SubmissionStatus::Late:      return "Late";
        case SubmissionStatus::Marked:    return "Marked";
        case SubmissionStatus::Moderated: return "Moderated";
        case SubmissionStatus::Released:  return "Released";
    }
    return "Unknown";
}

inline SubmissionStatus statusFromString(const std::string& s) {
    if (s == "Submitted") return SubmissionStatus::Submitted;
    if (s == "Late")      return SubmissionStatus::Late;
    if (s == "Marked")    return SubmissionStatus::Marked;
    if (s == "Moderated") return SubmissionStatus::Moderated;
    if (s == "Released")  return SubmissionStatus::Released;
    throw std::invalid_argument("Unknown status: " + s);
}

// Domain / application errors (never crash the UI)
class LmsError : public std::runtime_error {
public:
    explicit LmsError(const std::string& msg) : std::runtime_error(msg) {}
};

class AuthError : public LmsError {
public:
    explicit AuthError(const std::string& msg) : LmsError(msg) {}
};

class ValidationError : public LmsError {
public:
    explicit ValidationError(const std::string& msg) : LmsError(msg) {}
};

class PersistenceError : public LmsError {
public:
    explicit PersistenceError(const std::string& msg) : LmsError(msg) {}
};

} // namespace lms
