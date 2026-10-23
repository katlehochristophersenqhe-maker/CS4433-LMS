#include "lms/infra/Repositories.hpp"

namespace lms {

SqliteAuditRepository::SqliteAuditRepository(SqliteDatabase& db) : db_(db) {}

int SqliteAuditRepository::append(const AuditEntry& e) {
    SqliteStatement st(db_,
        "INSERT INTO Audit_Log (submission_id, changed_by, changed_at, old_mark, new_mark, reason) "
        "VALUES (?, ?, ?, ?, ?, ?)");
    st.bind(1, e.submissionId).bind(2, e.changedBy).bind(3, e.changedAt);
    if (e.oldMark) st.bind(4, *e.oldMark); else st.bindNull(4);
    st.bind(5, e.newMark).bind(6, e.reason);
    st.execute();
    return static_cast<int>(db_.lastInsertRowId());
}

std::vector<AuditEntry> SqliteAuditRepository::findBySubmission(int submissionId) {
    SqliteStatement st(db_,
        "SELECT log_id, submission_id, changed_by, changed_at, old_mark, new_mark, reason "
        "FROM Audit_Log WHERE submission_id = ? ORDER BY changed_at");
    st.bind(1, submissionId);
    std::vector<AuditEntry> out;
    while (st.step()) {
        AuditEntry e;
        e.id = st.columnInt(0);
        e.submissionId = st.columnInt(1);
        e.changedBy = st.columnInt(2);
        e.changedAt = st.columnText(3);
        if (!st.columnIsNull(4)) e.oldMark = st.columnDouble(4);
        e.newMark = st.columnDouble(5);
        e.reason = st.columnText(6);
        out.push_back(e);
    }
    return out;
}

} // namespace lms
