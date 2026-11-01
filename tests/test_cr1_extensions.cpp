#include "catch.hpp"
#include "lms/app/Services.hpp"
#include "lms/infra/Repositories.hpp"
#include <cstdio>
#include <filesystem>

using namespace lms;

namespace {

struct TestDatabaseCleanup {
    const std::string path = "test_cr1.db";

    TestDatabaseCleanup() {
        std::remove(path.c_str());
    }

    ~TestDatabaseCleanup() {
        std::remove(path.c_str());
    }
};

class Cr1Fixture {
public:
    Cr1Fixture()
        : db_(cleanup_.path),
          assignments_(db_),
          submissions_(db_),
          extensions_(db_),
          audit_(db_),
          courses_(db_),
          student_(assignments_, submissions_, extensions_, courses_, penalty_),
          lecturer_(assignments_, submissions_, extensions_, audit_, penalty_) {
        const std::string schemaPath = std::filesystem::exists("sql/schema.sql")
            ? "sql/schema.sql"
            : "../sql/schema.sql";
        db_.runSchemaFile(schemaPath);
        db_.exec(
            "INSERT INTO Department (department_id, name) VALUES (1, 'Mathematics');"
            "INSERT INTO User (user_id, username, password_hash, full_name, role, department_id) "
            "VALUES (10, 'lecturer', 'unused', 'Lecturer', 'Lecturer', 1);"
            "INSERT INTO User (user_id, username, password_hash, full_name, role) "
            "VALUES (20, 'student', 'unused', 'Student', 'Student');"
            "INSERT INTO Course (course_id, department_id, code, title, lecturer_id) "
            "VALUES (30, 1, 'MATH101', 'Mathematics', 10);"
            "INSERT INTO Enrolment (student_id, course_id) VALUES (20, 30);");
    }

    Assignment makeAssignment(const std::string& deadline) {
        Assignment assignment;
        assignment.courseId = 30;
        assignment.title = "Test assignment";
        assignment.deadline = deadline;
        assignment.maxMark = 100;
        assignment.id = assignments_.save(assignment);
        return assignment;
    }

    SqliteExtensionRepository& extensions() { return extensions_; }
    SqliteSubmissionRepository& submissions() { return submissions_; }
    StudentService& student() { return student_; }
    LecturerService& lecturer() { return lecturer_; }

private:
    TestDatabaseCleanup cleanup_;
    SqliteDatabase db_;
    SqliteAssignmentRepository assignments_;
    SqliteSubmissionRepository submissions_;
    SqliteExtensionRepository extensions_;
    SqliteAuditRepository audit_;
    SqliteCourseRepository courses_;
    StandardLatePenaltyStrategy penalty_;
    StudentService student_;
    LecturerService lecturer_;
};

} // namespace

TEST_CASE("CR1 – class-wide extension is visible and used for submission lateness",
          "[integration][extension][CR1]") {
    Cr1Fixture fixture;
    const auto assignment = fixture.makeAssignment("2000-01-01T00:00:00Z");
    const std::string extendedDeadline = "2099-01-01T00:00:00Z";

    fixture.lecturer().grantClassExtension(
        assignment.id, extendedDeadline, "School closure", 10);

    const auto extension = fixture.extensions().latestFor(assignment.id, 20);
    REQUIRE(extension.has_value());
    REQUIRE_FALSE(extension->studentId.has_value());
    REQUIRE(extension->newDeadline == extendedDeadline);

    const auto assignments = fixture.student().listAssignmentsWithDeadlines(20);
    REQUIRE(assignments.size() == 1);
    REQUIRE(assignments[0].assignment.deadline == assignment.deadline);
    REQUIRE(assignments[0].effectiveDeadline == extendedDeadline);

    fixture.student().submit(20, assignment.id, "Completed work");
    const auto submission = fixture.submissions().findFor(assignment.id, 20);
    REQUIRE(submission.has_value());
    REQUIRE(submission->status == SubmissionStatus::Submitted);
    REQUIRE(submission->daysLate == 0);
}

TEST_CASE("CR1 – individual extension takes precedence over class-wide extension",
          "[integration][extension][CR1]") {
    Cr1Fixture fixture;
    const auto assignment = fixture.makeAssignment("2000-01-01T00:00:00Z");

    fixture.lecturer().grantClassExtension(
        assignment.id, "2099-01-01T00:00:00Z", "School closure", 10);
    fixture.lecturer().grantExtension(
        assignment.id, 20, "2098-01-01T00:00:00Z", "Individual circumstances", 10);

    const auto extension = fixture.extensions().latestFor(assignment.id, 20);
    REQUIRE(extension.has_value());
    REQUIRE(extension->studentId == 20);

    const auto assignments = fixture.student().listAssignmentsWithDeadlines(20);
    REQUIRE(assignments.size() == 1);
    REQUIRE(assignments[0].effectiveDeadline == "2098-01-01T00:00:00Z");
}

TEST_CASE("CR1 – class-wide extension validates assignment, deadline, and reason",
          "[integration][extension][CR1]") {
    Cr1Fixture fixture;
    const auto assignment = fixture.makeAssignment("2099-01-01T00:00:00Z");

    REQUIRE_THROWS(fixture.lecturer().grantClassExtension(
        999, "2100-01-01T00:00:00Z", "Reason", 10));
    REQUIRE_THROWS(fixture.lecturer().grantClassExtension(
        assignment.id, "2100-01-01T00:00:00Z", "", 10));
    REQUIRE_THROWS(fixture.lecturer().grantClassExtension(
        assignment.id, assignment.deadline, "Reason", 10));
    REQUIRE_FALSE(fixture.extensions().latestFor(assignment.id, 20).has_value());
}

TEST_CASE("CR1 – startup migrates legacy extension tables and preserves rows",
          "[integration][extension][CR1][migration]") {
    const std::string path = "test_cr1_migration.db";
    std::remove(path.c_str());
    {
        SqliteDatabase db(path);
        db.runSchemaFile(std::filesystem::exists("sql/schema.sql")
            ? "sql/schema.sql"
            : "../sql/schema.sql");
        db.exec(
            "INSERT INTO Department (department_id, name) VALUES (1, 'Mathematics');"
            "INSERT INTO User (user_id, username, password_hash, full_name, role) "
            "VALUES (20, 'student', 'unused', 'Student', 'Student');"
            "INSERT INTO User (user_id, username, password_hash, full_name, role) "
            "VALUES (10, 'lecturer', 'unused', 'Lecturer', 'Lecturer');"
            "INSERT INTO Course (course_id, department_id, code, title) "
            "VALUES (30, 1, 'MATH101', 'Mathematics');"
            "INSERT INTO Assignment (assignment_id, course_id, title, deadline, max_mark) "
            "VALUES (40, 30, 'Test assignment', '2099-01-01T00:00:00Z', 100);"
            "ALTER TABLE Extension RENAME TO Extension_current;"
            "CREATE TABLE Extension ("
            " extension_id INTEGER PRIMARY KEY AUTOINCREMENT,"
            " assignment_id INTEGER NOT NULL REFERENCES Assignment(assignment_id),"
            " student_id INTEGER NOT NULL REFERENCES User(user_id),"
            " new_deadline TEXT NOT NULL,"
            " reason TEXT NOT NULL,"
            " granted_by INTEGER NOT NULL REFERENCES User(user_id)"
            ");"
            "INSERT INTO Extension VALUES "
            "(1, 40, 20, '2099-02-01T00:00:00Z', 'Existing extension', 10);"
            "DROP TABLE Extension_current;");

        db.ensureClassWideExtensionsSupported();
        db.ensureClassWideExtensionsSupported();

        SqliteStatement column(db, "PRAGMA table_info(Extension)");
        bool studentIdAllowsNull = false;
        while (column.step()) {
            if (column.columnText(1) == "student_id") {
                studentIdAllowsNull = column.columnInt(3) == 0;
                break;
            }
        }
        REQUIRE(studentIdAllowsNull);

        SqliteExtensionRepository extensions(db);
        const auto existing = extensions.latestFor(40, 20);
        REQUIRE(existing.has_value());
        REQUIRE(existing->id == 1);
        REQUIRE(existing->newDeadline == "2099-02-01T00:00:00Z");

        Extension classWide;
        classWide.assignmentId = 40;
        classWide.newDeadline = "2099-03-01T00:00:00Z";
        classWide.reason = "Class-wide after migration";
        classWide.grantedBy = 10;
        const int classWideId = extensions.add(classWide);
        SqliteStatement migratedRow(db,
            "SELECT student_id FROM Extension WHERE extension_id = ?");
        migratedRow.bind(1, classWideId);
        REQUIRE(migratedRow.step());
        REQUIRE(migratedRow.columnIsNull(0));
    }
    std::remove(path.c_str());
}
