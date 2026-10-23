#include "lms/infra/Repositories.hpp"

namespace lms {

SqliteExtensionRepository::SqliteExtensionRepository(SqliteDatabase& db) : db_(db) {}

std::optional<Extension> SqliteExtensionRepository::latestFor(int assignmentId, int studentId) {
    // Prefer individual extension; fall back to class-wide (student_id IS NULL)
    SqliteStatement st(db_,
        "SELECT extension_id, assignment_id, student_id, new_deadline, reason, granted_by "
        "FROM Extension WHERE assignment_id = ? AND (student_id = ? OR student_id IS NULL) "
        "ORDER BY CASE WHEN student_id IS NOT NULL THEN 0 ELSE 1 END, new_deadline DESC LIMIT 1");
    st.bind(1, assignmentId).bind(2, studentId);
    if (!st.step()) return std::nullopt;
    Extension e;
    e.id = st.columnInt(0);
    e.assignmentId = st.columnInt(1);
    if (!st.columnIsNull(2)) e.studentId = st.columnInt(2);
    e.newDeadline = st.columnText(3);
    e.reason = st.columnText(4);
    e.grantedBy = st.columnInt(5);
    return e;
}

int SqliteExtensionRepository::add(const Extension& e) {
    SqliteStatement st(db_,
        "INSERT INTO Extension (assignment_id, student_id, new_deadline, reason, granted_by) "
        "VALUES (?, ?, ?, ?, ?)");
    st.bind(1, e.assignmentId);
    if (e.studentId) st.bind(2, *e.studentId); else st.bindNull(2);
    st.bind(3, e.newDeadline).bind(4, e.reason).bind(5, e.grantedBy);
    st.execute();
    return static_cast<int>(db_.lastInsertRowId());
}

} // namespace lms
