#include "lms/infra/SqliteDatabase.hpp"
#include "lms/infra/PasswordHasher.hpp"
#include "lms/infra/Repositories.hpp"
#include "lms/domain/LatePenaltyStrategy.hpp"
#include "lms/app/Services.hpp"
#include "lms/ui/ConsoleUI.hpp"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

static void seedIfEmpty(lms::AdminService& admin,
                        lms::IUserRepository& users) {
    if (users.findByUsername("admin")) return; // already seeded

    std::cout << "Seeding demo data...\n";
    const int deptId = admin.createDepartment("Mathematics");
    const int adminId = admin.createUser("admin", "admin123", "System Administrator",
                                         lms::Role::Administrator);
    const int hodId = admin.createUser("hod1", "hod123", "Thabo HoD",
                                       lms::Role::HoD, deptId);
    const int lecId = admin.createUser("lec1", "lec123", "Lineo Lecturer",
                                       lms::Role::Lecturer, deptId);
    const int stuId = admin.createUser("stu1", "stu123", "Palesa Student",
                                       lms::Role::Student, 0, "S2021001");
    (void)adminId; (void)hodId;
    const int courseId = admin.createCourse(deptId, "MATH101", "Introduction to Algebra", lecId);
    admin.enrolStudent(stuId, courseId);
    std::cout << "Demo users: admin/admin123, hod1/hod123, lec1/lec123, stu1/stu123\n";
    std::cout << "Department id=" << deptId << " Course id=" << courseId << "\n";
}

int main(int argc, char** argv) {
    try {
        const std::string dbPath = (argc > 1) ? argv[1] : "lms.db";
        std::string schemaPath = "schema.sql";
        if (!fs::exists(schemaPath)) {
            // try relative to executable / source tree
            if (fs::exists("../sql/schema.sql")) schemaPath = "../sql/schema.sql";
            else if (fs::exists("sql/schema.sql")) schemaPath = "sql/schema.sql";
        }

        lms::SqliteDatabase db(dbPath);
        db.runSchemaFile(schemaPath);
        db.ensureClassWideExtensionsSupported();

        lms::Sha256PasswordHasher hasher;
        lms::StandardLatePenaltyStrategy penalty;

        lms::SqliteUserRepository userRepo(db);
        lms::SqliteSubmissionRepository submissionRepo(db);
        lms::SqliteAssignmentRepository assignmentRepo(db);
        lms::SqliteExtensionRepository extensionRepo(db);
        lms::SqliteAuditRepository auditRepo(db);
        lms::SqliteCourseRepository courseRepo(db);

        lms::AuthService auth(userRepo, hasher);
        lms::AdminService admin(userRepo, courseRepo, hasher);
        lms::StudentService student(assignmentRepo, submissionRepo, extensionRepo, courseRepo, penalty);
        lms::LecturerService lecturer(assignmentRepo, submissionRepo, extensionRepo, auditRepo, penalty);
        lms::HoDService hod(submissionRepo, auditRepo);

        seedIfEmpty(admin, userRepo);

        lms::ConsoleUI ui(auth, admin, student, lecturer, hod);
        ui.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }
}
