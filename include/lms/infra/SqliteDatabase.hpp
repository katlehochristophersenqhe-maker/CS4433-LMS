#pragma once
#include "lms/domain/Types.hpp"
#include <sqlite3.h>
#include <string>
#include <memory>

namespace lms {

// RAII wrapper around sqlite3*
class SqliteDatabase {
public:
    explicit SqliteDatabase(const std::string& path);
    ~SqliteDatabase();

    SqliteDatabase(const SqliteDatabase&) = delete;
    SqliteDatabase& operator=(const SqliteDatabase&) = delete;

    sqlite3* handle() const { return db_; }

    void exec(const std::string& sql);
    void applyPragmas();
    long long lastInsertRowId() const;

    // Run schema file (CREATE TABLE IF NOT EXISTS ...)
    void runSchemaFile(const std::string& path);
    void ensureClassWideExtensionsSupported();

private:
    sqlite3* db_ = nullptr;
};

// RAII prepared statement with parameter binding
class SqliteStatement {
public:
    SqliteStatement(const SqliteDatabase& db, const std::string& sql);
    ~SqliteStatement();

    SqliteStatement(const SqliteStatement&) = delete;
    SqliteStatement& operator=(const SqliteStatement&) = delete;

    SqliteStatement& bind(int idx, int value);
    SqliteStatement& bind(int idx, long long value);
    SqliteStatement& bind(int idx, double value);
    SqliteStatement& bind(int idx, const std::string& value);
    SqliteStatement& bindNull(int idx);

    bool step();          // true if a row is available
    void reset();
    void execute();       // step once, expect DONE

    int columnInt(int col) const;
    long long columnInt64(int col) const;
    double columnDouble(int col) const;
    std::string columnText(int col) const;
    bool columnIsNull(int col) const;

private:
    sqlite3_stmt* stmt_ = nullptr;
};

} // namespace lms
