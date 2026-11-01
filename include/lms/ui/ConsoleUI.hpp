#pragma once
#include "lms/app/Services.hpp"
#include <memory>

namespace lms {

class ConsoleUI {
public:
    ConsoleUI(AuthService& auth, AdminService& admin, StudentService& student,
              LecturerService& lecturer, HoDService& hod);
    void run();

private:
    void loginLoop();
    void adminMenu(Administrator& user);
    void studentMenu(const Student& user);
    void lecturerMenu(const Lecturer& user);
    void hodMenu(const HeadOfDepartment& user);

    static int readInt(const std::string& prompt);
    static std::string readLine(const std::string& prompt);

    AuthService& auth_;
    AdminService& admin_;
    StudentService& student_;
    LecturerService& lecturer_;
    HoDService& hod_;
};

} // namespace lms
