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
                        lms::IUserRepository& users,
                        lms::StudentService& student,
                        lms::LecturerService& lecturer,
                        lms::HoDService& hod,
                        lms::ISubmissionRepository& submissions) {
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
    const int stu2Id = admin.createUser("stu2", "stu123", "Mpho Student",
                                        lms::Role::Student, 0, "S2021002");
    const int stu3Id = admin.createUser("stu3", "stu123", "Nthabeleng Student",
                                        lms::Role::Student, 0, "S2021003");
    (void)adminId; (void)hodId;
    const int courseId = admin.createCourse(deptId, "MATH101", "Introduction to Algebra", lecId);
    admin.enrolStudent(stuId, courseId);
    admin.enrolStudent(stu2Id, courseId);
    admin.enrolStudent(stu3Id, courseId);

    const int worksheetId = lecturer.createAssignment(
        courseId, "Algebra Foundations Worksheet",
        "Solve the linear equations and show your working.",
        "2099-12-15T23:59:59Z", 100.0);
    const int quizId = lecturer.createAssignment(
        courseId, "Linear Equations Quiz",
        "Complete all questions and explain one solution method.",
        "2099-12-20T23:59:59Z", 50.0);
    lecturer.createAssignment(
        courseId, "Functions Mini Project",
        "Model a real-world relationship using a function and interpret its graph.",
        "2099-12-22T23:59:59Z", 100.0);

    student.submit(stuId, worksheetId,
                   "1) x = 4. 2) x = -2. 3) The equations are solved by isolating x.");
    student.submit(stu2Id, worksheetId,
                   "Completed solutions with substitution checks for each equation.");
    student.submit(stu3Id, worksheetId,
                   "Solved each equation and verified the results by substitution.");
    student.submit(stuId, quizId,
                   "Used inverse operations to solve each equation and checked the answers.");
    student.submit(stu2Id, quizId,
                   "Solutions include working for every question and a substitution check.");
    student.submit(stu3Id, quizId,
                   "Completed all quiz questions and included the requested explanation.");

    const auto releasedSubmission = submissions.findFor(worksheetId, stuId);
    if (!releasedSubmission) {
        throw lms::PersistenceError("Could not find the seeded worksheet submission");
    }
    lecturer.markSubmission(releasedSubmission->id, 86.0,
                            "Clear working. Review sign changes in question 3.", lecId);
    hod.release(releasedSubmission->id);

    const auto secondReleasedSubmission = submissions.findFor(quizId, stu2Id);
    if (!secondReleasedSubmission) {
        throw lms::PersistenceError("Could not find the seeded quiz submission");
    }
    lecturer.markSubmission(secondReleasedSubmission->id, 47.0,
                            "Excellent solutions with clear explanations.", lecId);
    hod.release(secondReleasedSubmission->id);

    const auto markedSubmission = submissions.findFor(quizId, stu3Id);
    if (!markedSubmission) {
        throw lms::PersistenceError("Could not find the seeded quiz submission");
    }
    lecturer.markSubmission(markedSubmission->id, 42.0,
                            "Good method. Check the arithmetic in question 4.", lecId);

    std::cout << "Demo users: admin/admin123, hod1/hod123, lec1/lec123, "
                 "stu1/stu123, stu2/stu123, stu3/stu123\n";
    std::cout << "Sample records include six submissions across three assignments: "
                 "three awaiting marking, one marked, and two released.\n";
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

        seedIfEmpty(admin, userRepo, student, lecturer, hod, submissionRepo);

        lms::ConsoleUI ui(auth, admin, student, lecturer, hod);
        ui.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }
}
