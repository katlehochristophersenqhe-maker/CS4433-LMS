#include "lms/infra/SqliteDatabase.hpp"
#include <fstream>
#include <sstream>

namespace lms {

SqliteDatabase::SqliteDatabase(const std::string& path) {
    if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK) {
        std::string err = db_ ? sqlite3_errmsg(db_) : "unknown";
        if (db_) sqlite3_close(db_);
        throw PersistenceError("Cannot open database: " + err);
    }
    applyPragmas();
}

SqliteDatabase::~SqliteDatabase() {
    if (db_) sqlite3_close(db_);
}

void SqliteDatabase::applyPragmas() {
    exec("PRAGMA foreign_keys = ON;");
    exec("PRAGMA journal_mode = DELETE;");
    exec("PRAGMA busy_timeout = 5000;");
}

void SqliteDatabase::exec(const std::string& sql) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::string err = errMsg ? errMsg : "exec failed";
        sqlite3_free(errMsg);
        throw PersistenceError(err);
    }
}

long long SqliteDatabase::lastInsertRowId() const {
    return sqlite3_last_insert_rowid(db_);
}

void SqliteDatabase::runSchemaFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw PersistenceError("Cannot open schema file: " + path);
    std::ostringstream ss;
    ss << in.rdbuf();
    exec(ss.str());
}

// ---- Statement ----

SqliteStatement::SqliteStatement(const SqliteDatabase& db, const std::string& sql) {
    if (sqlite3_prepare_v2(db.handle(), sql.c_str(), -1, &stmt_, nullptr) != SQLITE_OK) {
        throw PersistenceError(std::string("prepare failed: ") + sqlite3_errmsg(db.handle()));
    }
}

SqliteStatement::~SqliteStatement() {
    if (stmt_) sqlite3_finalize(stmt_);
}

SqliteStatement& SqliteStatement::bind(int idx, int value) {
    if (sqlite3_bind_int(stmt_, idx, value) != SQLITE_OK)
        throw PersistenceError("bind int failed");
    return *this;
}

SqliteStatement& SqliteStatement::bind(int idx, long long value) {
    if (sqlite3_bind_int64(stmt_, idx, value) != SQLITE_OK)
        throw PersistenceError("bind int64 failed");
    return *this;
}

SqliteStatement& SqliteStatement::bind(int idx, double value) {
    if (sqlite3_bind_double(stmt_, idx, value) != SQLITE_OK)
        throw PersistenceError("bind double failed");
    return *this;
}

SqliteStatement& SqliteStatement::bind(int idx, const std::string& value) {
    if (sqlite3_bind_text(stmt_, idx, value.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
        throw PersistenceError("bind text failed");
    return *this;
}

SqliteStatement& SqliteStatement::bindNull(int idx) {
    if (sqlite3_bind_null(stmt_, idx) != SQLITE_OK)
        throw PersistenceError("bind null failed");
    return *this;
}

bool SqliteStatement::step() {
    const int rc = sqlite3_step(stmt_);
    if (rc == SQLITE_ROW) return true;
    if (rc == SQLITE_DONE) return false;
    throw PersistenceError(std::string("step failed: ") + sqlite3_errstr(rc));
}

void SqliteStatement::reset() {
    sqlite3_reset(stmt_);
    sqlite3_clear_bindings(stmt_);
}

void SqliteStatement::execute() {
    if (step()) {
        // unexpected row
    }
}

int SqliteStatement::columnInt(int col) const { return sqlite3_column_int(stmt_, col); }
long long SqliteStatement::columnInt64(int col) const { return sqlite3_column_int64(stmt_, col); }
double SqliteStatement::columnDouble(int col) const { return sqlite3_column_double(stmt_, col); }
bool SqliteStatement::columnIsNull(int col) const { return sqlite3_column_type(stmt_, col) == SQLITE_NULL; }

std::string SqliteStatement::columnText(int col) const {
    const unsigned char* t = sqlite3_column_text(stmt_, col);
    return t ? reinterpret_cast<const char*>(t) : "";
}

} // namespace lms
