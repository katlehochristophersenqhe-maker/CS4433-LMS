#include "catch.hpp"
#include "lms/infra/PasswordHasher.hpp"

using namespace lms;

TEST_CASE("Password hasher – hash is not plain text", "[unit][security]") {
    Sha256PasswordHasher h;
    std::string stored = h.hash("secret123");
    REQUIRE(stored.find("secret123") == std::string::npos);
    REQUIRE(stored.find('$') != std::string::npos);
}

TEST_CASE("Password hasher – verify accepts correct password", "[unit][security]") {
    Sha256PasswordHasher h;
    std::string stored = h.hash("mypass");
    REQUIRE(h.verify("mypass", stored));
}

TEST_CASE("Password hasher – verify rejects wrong password", "[unit][security]") {
    Sha256PasswordHasher h;
    std::string stored = h.hash("mypass");
    REQUIRE_FALSE(h.verify("wrong", stored));
    REQUIRE_FALSE(h.verify("", stored));
}

TEST_CASE("Password hasher – same password produces different salts", "[unit][security]") {
    Sha256PasswordHasher h;
    std::string a = h.hash("same");
    std::string b = h.hash("same");
    // salts should differ so full strings differ
    REQUIRE(a != b);
    REQUIRE(h.verify("same", a));
    REQUIRE(h.verify("same", b));
}

TEST_CASE("Password hasher – malformed stored hash fails verify", "[unit][security][exception]") {
    Sha256PasswordHasher h;
    REQUIRE_FALSE(h.verify("x", "nosecondpart"));
    REQUIRE_FALSE(h.verify("x", ""));
}
