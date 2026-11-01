#include "lms/infra/Repositories.hpp"

namespace lms {

SqliteUserRepository::SqliteUserRepository(SqliteDatabase& db) : db_(db) {}

std::unique_ptr<User> SqliteUserRepository::mapRow(const SqliteStatement& st) {
    const int id = st.columnInt(0);
    const std::string username = st.columnText(1);
    const std::string fullName = st.columnText(3);
    const Role role = roleFromString(st.columnText(4));
    const bool active = st.columnInt(5) != 0;
    const int deptId = st.columnIsNull(6) ? 0 : st.columnInt(6);
    const std::string studentNumber = st.columnIsNull(7) ? "" : st.columnText(7);
    return UserFactory::create(role, id, username, fullName, deptId, studentNumber, active);
}

std::unique_ptr<User> SqliteUserRepository::findById(int id) {
    SqliteStatement st(db_,
        "SELECT user_id, username, password_hash, full_name, role, is_active, department_id, student_number "
        "FROM User WHERE user_id = ?");
    st.bind(1, id);
    if (!st.step()) return nullptr;
    return mapRow(st);
}

std::unique_ptr<User> SqliteUserRepository::findByUsername(const std::string& username) {
    SqliteStatement st(db_,
        "SELECT user_id, username, password_hash, full_name, role, is_active, department_id, student_number "
        "FROM User WHERE username = ? COLLATE NOCASE");
    st.bind(1, username);
    if (!st.step()) return nullptr;
    return mapRow(st);
}

std::string SqliteUserRepository::passwordHash(int userId) {
    SqliteStatement st(db_, "SELECT password_hash FROM User WHERE user_id = ?");
    st.bind(1, userId);
    if (!st.step()) throw PersistenceError("User not found for password lookup");
    return st.columnText(0);
}

int SqliteUserRepository::save(const std::string& username, const std::string& hash,
                               const std::string& fullName, Role role,
                               int departmentId, const std::string& studentNumber) {
    SqliteStatement st(db_,
        "INSERT INTO User (username, password_hash, full_name, role, is_active, department_id, student_number) "
        "VALUES (?, ?, ?, ?, 1, ?, ?)");
    st.bind(1, username).bind(2, hash).bind(3, fullName).bind(4, toString(role));
    if (departmentId > 0) st.bind(5, departmentId); else st.bindNull(5);
    if (!studentNumber.empty()) st.bind(6, studentNumber); else st.bindNull(6);
    st.execute();
    return static_cast<int>(db_.lastInsertRowId());
}

} // namespace lms
