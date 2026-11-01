#include "lms/infra/Repositories.hpp"

namespace lms {

SqliteSubmissionRepository::SqliteSubmissionRepository(SqliteDatabase& db) : db_(db) {}

Submission SqliteSubmissionRepository::mapRow(const SqliteStatement& st) {
    Submission s;
    s.id = st.columnInt(0);
    s.assignmentId = st.columnInt(1);
    s.studentId = st.columnInt(2);
    s.content = st.columnText(3);
    s.submittedAt = st.columnText(4);
    s.status = statusFromString(st.columnText(5));
    if (!st.columnIsNull(6)) s.rawMark = st.columnDouble(6);
    if (!st.columnIsNull(7)) s.feedback = st.columnText(7);
    if (!st.columnIsNull(8)) s.finalMark = st.columnDouble(8);
    s.daysLate = st.columnInt(9);
    return s;
}

std::optional<Submission> SqliteSubmissionRepository::findById(int id) {
    SqliteStatement st(db_,
        "SELECT submission_id, assignment_id, student_id, content, submitted_at, status, "
        "raw_mark, feedback, final_mark, days_late FROM Submission WHERE submission_id = ?");
    st.bind(1, id);
    if (!st.step()) return std::nullopt;
    return mapRow(st);
}

std::optional<Submission> SqliteSubmissionRepository::findFor(int assignmentId, int studentId) {
    SqliteStatement st(db_,
        "SELECT submission_id, assignment_id, student_id, content, submitted_at, status, "
        "raw_mark, feedback, final_mark, days_late FROM Submission "
        "WHERE assignment_id = ? AND student_id = ?");
    st.bind(1, assignmentId).bind(2, studentId);
    if (!st.step()) return std::nullopt;
    return mapRow(st);
}

std::vector<SubmissionDetails> SqliteSubmissionRepository::findForLecturer(int lecturerId) {
    SqliteStatement st(db_,
        "SELECT s.submission_id, s.assignment_id, s.student_id, s.content, s.submitted_at, s.status, "
        "s.raw_mark, s.feedback, s.final_mark, s.days_late, a.title, c.code, u.full_name, u.username "
        "FROM Submission s "
        "JOIN Assignment a ON a.assignment_id = s.assignment_id "
        "JOIN Course c ON c.course_id = a.course_id "
        "JOIN User u ON u.user_id = s.student_id "
        "WHERE c.lecturer_id = ? "
        "ORDER BY a.title, u.full_name, s.submission_id");
    st.bind(1, lecturerId);

    std::vector<SubmissionDetails> out;
    while (st.step()) {
        SubmissionDetails details;
        details.submission = mapRow(st);
        details.assignmentTitle = st.columnText(10);
        details.courseCode = st.columnText(11);
        details.studentName = st.columnText(12);
        details.studentUsername = st.columnText(13);
        out.push_back(std::move(details));
    }
    return out;
}

std::vector<Submission> SqliteSubmissionRepository::findByAssignmentAndStatus(
    int assignmentId, SubmissionStatus status) {
    SqliteStatement st(db_,
        "SELECT submission_id, assignment_id, student_id, content, submitted_at, status, "
        "raw_mark, feedback, final_mark, days_late FROM Submission "
        "WHERE assignment_id = ? AND status = ?");
    st.bind(1, assignmentId).bind(2, toString(status));
    std::vector<Submission> out;
    while (st.step()) out.push_back(mapRow(st));
    return out;
}

std::vector<Submission> SqliteSubmissionRepository::findByDepartmentAndStatus(
    int departmentId, SubmissionStatus status) {
    SqliteStatement st(db_,
        "SELECT s.submission_id, s.assignment_id, s.student_id, s.content, s.submitted_at, s.status, "
        "s.raw_mark, s.feedback, s.final_mark, s.days_late "
        "FROM Submission s "
        "JOIN Assignment a ON a.assignment_id = s.assignment_id "
        "JOIN Course c ON c.course_id = a.course_id "
        "WHERE c.department_id = ? AND s.status = ?");
    st.bind(1, departmentId).bind(2, toString(status));
    std::vector<Submission> out;
    while (st.step()) out.push_back(mapRow(st));
    return out;
}

std::vector<Submission> SqliteSubmissionRepository::findByStudentAndStatus(
    int studentId, SubmissionStatus status) {
    SqliteStatement st(db_,
        "SELECT submission_id, assignment_id, student_id, content, submitted_at, status, "
        "raw_mark, feedback, final_mark, days_late FROM Submission "
        "WHERE student_id = ? AND status = ?");
    st.bind(1, studentId).bind(2, toString(status));
    std::vector<Submission> out;
    while (st.step()) out.push_back(mapRow(st));
    return out;
}

void SqliteSubmissionRepository::save(Submission& s) {
    SqliteStatement st(db_,
        "INSERT INTO Submission (assignment_id, student_id, content, submitted_at, status, "
        "days_late) VALUES (?, ?, ?, ?, ?, ?) "
        "ON CONFLICT(assignment_id, student_id) DO UPDATE SET "
        "content=excluded.content, submitted_at=excluded.submitted_at, "
        "status=excluded.status, days_late=excluded.days_late, "
        "raw_mark=NULL, feedback=NULL, final_mark=NULL");
    st.bind(1, s.assignmentId)
      .bind(2, s.studentId)
      .bind(3, s.content)
      .bind(4, s.submittedAt)
      .bind(5, toString(s.status))
      .bind(6, s.daysLate);
    st.execute();

    // Reload id
    auto existing = findFor(s.assignmentId, s.studentId);
    if (existing) s.id = existing->id;
}

void SqliteSubmissionRepository::updateMarkAndStatus(
    int id, double rawMark, double finalMark,
    const std::string& feedback, SubmissionStatus status) {
    SqliteStatement st(db_,
        "UPDATE Submission SET raw_mark=?, final_mark=?, feedback=?, status=? "
        "WHERE submission_id=?");
    st.bind(1, rawMark).bind(2, finalMark).bind(3, feedback)
      .bind(4, toString(status)).bind(5, id);
    st.execute();
}

void SqliteSubmissionRepository::setStatus(int id, SubmissionStatus status) {
    SqliteStatement st(db_, "UPDATE Submission SET status=? WHERE submission_id=?");
    st.bind(1, toString(status)).bind(2, id);
    st.execute();
}

} // namespace lms
