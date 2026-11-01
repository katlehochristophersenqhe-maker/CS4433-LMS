#include "catch.hpp"
#include "lms/app/Services.hpp"
#include "lms/infra/Repositories.hpp"
#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <utility>

using namespace lms;

namespace {

struct TestDatabaseCleanup {
    explicit TestDatabaseCleanup(std::string file, bool removeOnDestruction = true)
        : path(std::move(file)), removeOnDestruction(removeOnDestruction) {
        std::remove(path.c_str());
    }
    const std::string path;
    const bool removeOnDestruction;

    ~TestDatabaseCleanup() {
        if (removeOnDestruction) {
            std::remove(path.c_str());
        }
    }
};

class Cr1Fixture {
public:
    explicit Cr1Fixture(const std::string& dbPath = "test_cr1.db",
                        bool removeDatabaseOnDestruction = true)
        : cleanup_(dbPath, removeDatabaseOnDestruction),
          db_(cleanup_.path),
          assignments_(db_),
          submissions_(db_),
          extensions_(db_),
          audit_(db_),
          courses_(db_),
          student_(assignments_, submissions_, extensions_, courses_, penalty_),
          lecturer_(assignments_, submissions_, extensions_, audit_, penalty_),
          hod_(submissions_, audit_) {
        const std::string schemaPath = std::filesystem::exists("sql/schema.sql")
            ? "sql/schema.sql"
            : "../sql/schema.sql";
        db_.runSchemaFile(schemaPath);
        db_.exec(
            "INSERT INTO Department (department_id, name) VALUES (1, 'Mathematics');"
            "INSERT INTO User (user_id, username, password_hash, full_name, role, department_id) "
            "VALUES (10, 'lecturer', 'unused', 'Lecturer', 'Lecturer', 1);"
            "INSERT INTO User (user_id, username, password_hash, full_name, role, department_id) "
            "VALUES (11, 'hod', 'unused', 'HoD', 'HoD', 1);"
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
    SqliteAuditRepository& audit() { return audit_; }
    StudentService& student() { return student_; }
    LecturerService& lecturer() { return lecturer_; }
    HoDService& hod() { return hod_; }

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
    HoDService hod_;
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

TEST_CASE("Lecturer can browse submissions for their assigned courses",
          "[integration][lecturer][submissions]") {
    Cr1Fixture fixture;
    const auto assignment = fixture.makeAssignment("2099-01-01T00:00:00Z");
    fixture.student().submit(20, assignment.id, "My submitted solution");

    const auto submissions = fixture.lecturer().listSubmissionsForLecturer(10);
    REQUIRE(submissions.size() == 1);
    REQUIRE(submissions[0].submission.assignmentId == assignment.id);
    REQUIRE(submissions[0].submission.studentId == 20);
    REQUIRE(submissions[0].submission.content == "My submitted solution");
    REQUIRE(submissions[0].assignmentTitle == "Test assignment");
    REQUIRE(submissions[0].courseCode == "MATH101");
    REQUIRE(submissions[0].studentName == "Student");
    REQUIRE(submissions[0].studentUsername == "student");
    REQUIRE(fixture.lecturer().listSubmissionsForLecturer(999).empty());
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

TEST_CASE("System – resubmission replaces unmarked work but is rejected after marking",
          "[system][submission]") {
    Cr1Fixture fixture;
    const auto assignment = fixture.makeAssignment("2099-01-01T00:00:00Z");

    fixture.student().submit(20, assignment.id, "First version");
    const auto first = fixture.submissions().findFor(assignment.id, 20);
    REQUIRE(first.has_value());

    fixture.student().submit(20, assignment.id, "Revised version");
    const auto revised = fixture.submissions().findFor(assignment.id, 20);
    REQUIRE(revised.has_value());
    REQUIRE(revised->id == first->id);
    REQUIRE(revised->content == "Revised version");
    REQUIRE(revised->status == SubmissionStatus::Submitted);

    fixture.lecturer().markSubmission(revised->id, 85.0, "Good work", 10);
    REQUIRE_THROWS(fixture.student().submit(20, assignment.id, "Late revision"));
    const auto marked = fixture.submissions().findFor(assignment.id, 20);
    REQUIRE(marked.has_value());
    REQUIRE(marked->content == "Revised version");
    REQUIRE(marked->status == SubmissionStatus::Marked);
}

TEST_CASE("System – marked work remains hidden until HoD release",
          "[system][marks][release]") {
    Cr1Fixture fixture;
    const auto assignment = fixture.makeAssignment("2099-01-01T00:00:00Z");
    fixture.student().submit(20, assignment.id, "Assessment work");
    const auto submission = fixture.submissions().findFor(assignment.id, 20);
    REQUIRE(submission.has_value());

    REQUIRE(fixture.student().listReleasedSubmissions(20).empty());
    fixture.lecturer().markSubmission(submission->id, 76.0, "Well done", 10);
    REQUIRE(fixture.student().listReleasedSubmissions(20).empty());

    fixture.hod().release(submission->id);
    const auto released = fixture.student().listReleasedSubmissions(20);
    REQUIRE(released.size() == 1);
    REQUIRE(released[0].id == submission->id);
    REQUIRE(released[0].isVisibleToStudent());
    REQUIRE(released[0].finalMark == 76.0);
    REQUIRE(released[0].feedback == "Well done");
}

TEST_CASE("System – mark-change audit records persist across database reopen",
          "[system][audit]") {
    const std::string path = "test_audit_persistence.db";
    TestDatabaseCleanup cleanup(path);
    int submissionId = 0;

    {
        Cr1Fixture fixture(path, false);
        const auto assignment = fixture.makeAssignment("2099-01-01T00:00:00Z");
        fixture.student().submit(20, assignment.id, "Assessment work");
        const auto submission = fixture.submissions().findFor(assignment.id, 20);
        REQUIRE(submission.has_value());
        submissionId = submission->id;

        fixture.lecturer().markSubmission(submissionId, 72.0, "Initial mark", 10);
        fixture.hod().moderate(submissionId, 68.0, "Correction after review", 11);

        const auto auditEntries = fixture.audit().findBySubmission(submissionId);
        REQUIRE(auditEntries.size() == 2);
    }

    {
        SqliteDatabase reopened(path);
        SqliteAuditRepository audit(reopened);
        const auto entries = audit.findBySubmission(submissionId);
        REQUIRE(entries.size() == 2);
        const auto initialMark = std::find_if(entries.begin(), entries.end(),
            [](const AuditEntry& entry) {
                return entry.reason == "Initial mark by lecturer";
            });
        REQUIRE(initialMark != entries.end());
        REQUIRE(initialMark->submissionId == submissionId);
        REQUIRE(initialMark->changedBy == 10);
        REQUIRE_FALSE(initialMark->oldMark.has_value());
        REQUIRE(initialMark->newMark == 72.0);

        const auto moderation = std::find_if(entries.begin(), entries.end(),
            [](const AuditEntry& entry) {
                return entry.reason == "Correction after review";
            });
        REQUIRE(moderation != entries.end());
        REQUIRE(moderation->submissionId == submissionId);
        REQUIRE(moderation->changedBy == 11);
        REQUIRE(moderation->oldMark == 72.0);
        REQUIRE(moderation->newMark == 68.0);
    }
}
