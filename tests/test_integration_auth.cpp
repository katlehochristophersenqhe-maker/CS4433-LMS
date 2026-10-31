#include "catch.hpp"
#include "lms/infra/SqliteDatabase.hpp"
#include "lms/infra/PasswordHasher.hpp"
#include "lms/infra/Repositories.hpp"
#include "lms/app/Services.hpp"
#include <cstdio>

using namespace lms;

static std::string testDbPath() {
    return "test_auth.db";
}

TEST_CASE("Integration – create user and login", "[integration][auth]") {
    std::remove(testDbPath().c_str());
    SqliteDatabase db(testDbPath());
    // minimal schema
    db.exec(
        "CREATE TABLE Department (department_id INTEGER PRIMARY KEY, name TEXT);"
        "CREATE TABLE User ("
        " user_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        " username TEXT UNIQUE NOT NULL,"
        " password_hash TEXT NOT NULL,"
        " full_name TEXT NOT NULL,"
        " role TEXT NOT NULL,"
        " is_active INTEGER NOT NULL DEFAULT 1,"
        " department_id INTEGER,"
        " student_number TEXT);"
    );

    Sha256PasswordHasher hasher;
    SqliteUserRepository users(db);
    SqliteCourseRepository courses(db);
    AdminService admin(users, courses, hasher);
    AuthService auth(users, hasher);

    int id = admin.createUser("testuser", "pass123", "Test User", Role::Student, 0, "S9");
    REQUIRE(id > 0);

    auto logged = auth.login("testuser", "pass123");
    REQUIRE(logged != nullptr);
    REQUIRE(logged->username() == "testuser");
    REQUIRE(logged->getRole() == Role::Student);

    REQUIRE_THROWS(auth.login("testuser", "wrong"));
    REQUIRE_THROWS(auth.login("nobody", "pass123"));

    std::remove(testDbPath().c_str());
}

TEST_CASE("Integration – password not stored plain", "[integration][security]") {
    std::remove("test_hash.db");
    SqliteDatabase db("test_hash.db");
    db.exec(
        "CREATE TABLE User ("
        " user_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        " username TEXT UNIQUE NOT NULL,"
        " password_hash TEXT NOT NULL,"
        " full_name TEXT NOT NULL,"
        " role TEXT NOT NULL,"
        " is_active INTEGER NOT NULL DEFAULT 1,"
        " department_id INTEGER,"
        " student_number TEXT);"
    );
    Sha256PasswordHasher hasher;
    SqliteUserRepository users(db);
    std::string hash = hasher.hash("plainSecret");
    users.save("u1", hash, "U", Role::Student, 0, "S1");
    std::string stored = users.passwordHash(1);
    REQUIRE(stored.find("plainSecret") == std::string::npos);
    REQUIRE(hasher.verify("plainSecret", stored));
    std::remove("test_hash.db");
}
