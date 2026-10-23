#include "lms/ui/ConsoleUI.hpp"
#include <iostream>
#include <limits>

namespace lms {

ConsoleUI::ConsoleUI(AuthService& auth, AdminService& admin, StudentService& student,
                     LecturerService& lecturer, HoDService& hod)
    : auth_(auth), admin_(admin), student_(student), lecturer_(lecturer), hod_(hod) {}

std::string ConsoleUI::readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

int ConsoleUI::readInt(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        int v = 0;
        if (std::cin >> v) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return v;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Please enter a number.\n";
    }
}

void ConsoleUI::run() {
    std::cout << "========================================\n";
    std::cout << "  Moderated Learning Management System\n";
    std::cout << "========================================\n";
    loginLoop();
}

void ConsoleUI::loginLoop() {
    while (true) {
        try {
            const std::string user = readLine("Username (or 'quit'): ");
            if (user == "quit" || user == "exit") break;
            const std::string pass = readLine("Password: ");
            auto logged = auth_.login(user, pass);
            std::cout << "Welcome, " << logged->fullName()
                      << " (" << toString(logged->getRole()) << ")\n";
            switch (logged->getRole()) {
                case Role::Administrator:
                    adminMenu(static_cast<Administrator&>(*logged)); break;
                case Role::Student:
                    studentMenu(static_cast<Student&>(*logged)); break;
                case Role::Lecturer:
                    lecturerMenu(static_cast<Lecturer&>(*logged)); break;
                case Role::HoD:
                    hodMenu(static_cast<HeadOfDepartment&>(*logged)); break;
            }
        } catch (const LmsError& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
}

void ConsoleUI::adminMenu(Administrator&) {
    while (true) {
        std::cout << "\n--- Administrator ---\n"
                  << "1. Create department\n2. Create user\n3. Create course\n"
                  << "4. Enrol student\n0. Logout\n";
        int c = readInt("Choice: ");
        try {
            if (c == 0) return;
            if (c == 1) {
                auto name = readLine("Department name: ");
                int id = admin_.createDepartment(name);
                std::cout << "Created department id=" << id << "\n";
            } else if (c == 2) {
                auto un = readLine("Username: ");
                auto pw = readLine("Password: ");
                auto fn = readLine("Full name: ");
                auto roleStr = readLine("Role (Student/Lecturer/HoD/Administrator): ");
                int dept = readInt("Department id (0 if none): ");
                auto sn = readLine("Student number (blank if not student): ");
                int id = admin_.createUser(un, pw, fn, roleFromString(roleStr), dept, sn);
                std::cout << "Created user id=" << id << "\n";
            } else if (c == 3) {
                int dept = readInt("Department id: ");
                auto code = readLine("Course code: ");
                auto title = readLine("Title: ");
                int lec = readInt("Lecturer user id (0 if none): ");
                int id = admin_.createCourse(dept, code, title, lec);
                std::cout << "Created course id=" << id << "\n";
            } else if (c == 4) {
                int sid = readInt("Student user id: ");
                int cid = readInt("Course id: ");
                admin_.enrolStudent(sid, cid);
                std::cout << "Enrolled.\n";
            }
        } catch (const LmsError& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
}

void ConsoleUI::studentMenu(Student& user) {
    while (true) {
        std::cout << "\n--- Student ---\n"
                  << "1. List my assignments\n2. Submit work\n0. Logout\n";
        int c = readInt("Choice: ");
        try {
            if (c == 0) return;
            if (c == 1) {
                auto list = student_.listAssignments(user.id());
                if (list.empty()) std::cout << "(no assignments)\n";
                for (const auto& a : list) {
                    std::cout << "  [" << a.id << "] " << a.title
                              << "  deadline=" << a.deadline
                              << "  max=" << a.maxMark << "\n";
                }
            } else if (c == 2) {
                int aid = readInt("Assignment id: ");
                auto content = readLine("Submission text: ");
                student_.submit(user.id(), aid, content);
                std::cout << "Submitted.\n";
            }
        } catch (const LmsError& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
}

void ConsoleUI::lecturerMenu(Lecturer& user) {
    while (true) {
        std::cout << "\n--- Lecturer ---\n"
                  << "1. Create assignment\n2. List submissions\n3. Mark submission\n"
                  << "4. Grant extension\n0. Logout\n";
        int c = readInt("Choice: ");
        try {
            if (c == 0) return;
            if (c == 1) {
                int cid = readInt("Course id: ");
                auto title = readLine("Title: ");
                auto desc = readLine("Description: ");
                auto dl = readLine("Deadline (YYYY-MM-DDTHH:MM:SSZ): ");
                double maxM = 0;
                std::cout << "Max mark: ";
                std::cin >> maxM;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                int id = lecturer_.createAssignment(cid, title, desc, dl, maxM);
                std::cout << "Created assignment id=" << id << "\n";
            } else if (c == 2) {
                int aid = readInt("Assignment id: ");
                auto list = lecturer_.listSubmissions(aid);
                for (const auto& s : list) {
                    std::cout << "  sub#" << s.id << " student=" << s.studentId
                              << " status=" << toString(s.status)
                              << " daysLate=" << s.daysLate << "\n";
                }
            } else if (c == 3) {
                int sid = readInt("Submission id: ");
                double mark = 0;
                std::cout << "Raw mark: ";
                std::cin >> mark;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                auto fb = readLine("Feedback: ");
                lecturer_.markSubmission(sid, mark, fb, user.id());
                std::cout << "Marked.\n";
            } else if (c == 4) {
                int aid = readInt("Assignment id: ");
                int stid = readInt("Student id: ");
                auto nd = readLine("New deadline (YYYY-MM-DDTHH:MM:SSZ): ");
                auto reason = readLine("Reason: ");
                lecturer_.grantExtension(aid, stid, nd, reason, user.id());
                std::cout << "Extension granted.\n";
            }
        } catch (const LmsError& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
}

void ConsoleUI::hodMenu(HeadOfDepartment& user) {
    while (true) {
        std::cout << "\n--- Head of Department ---\n"
                  << "1. View marked submissions\n2. Moderate mark\n3. Release mark\n0. Logout\n";
        int c = readInt("Choice: ");
        try {
            if (c == 0) return;
            if (c == 1) {
                auto list = hod_.markedInDepartment(user.departmentId());
                for (const auto& s : list) {
                    std::cout << "  sub#" << s.id << " student=" << s.studentId
                              << " final=" << (s.finalMark ? std::to_string(*s.finalMark) : "-")
                              << " status=" << toString(s.status) << "\n";
                }
            } else if (c == 2) {
                int sid = readInt("Submission id: ");
                auto change = readLine("Change mark? (y/n): ");
                if (change == "y" || change == "Y") {
                    double nm = 0;
                    std::cout << "New mark: ";
                    std::cin >> nm;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    auto reason = readLine("Reason: ");
                    hod_.moderate(sid, nm, reason, user.id());
                } else {
                    hod_.moderate(sid, std::nullopt, "", user.id());
                }
                std::cout << "Moderated.\n";
            } else if (c == 3) {
                int sid = readInt("Submission id: ");
                hod_.release(sid);
                std::cout << "Released.\n";
            }
        } catch (const LmsError& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
}

} // namespace lms
