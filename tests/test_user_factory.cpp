#include "catch.hpp"
#include "lms/domain/User.hpp"

using namespace lms;

TEST_CASE("UserFactory creates Student", "[unit][factory]") {
    auto u = UserFactory::create(Role::Student, 1, "stu", "Palesa", 0, "S001");
    REQUIRE(u != nullptr);
    REQUIRE(u->getRole() == Role::Student);
    REQUIRE(u->username() == "stu");
}

TEST_CASE("UserFactory creates Lecturer", "[unit][factory]") {
    auto u = UserFactory::create(Role::Lecturer, 2, "lec", "Lineo", 5);
    REQUIRE(u->getRole() == Role::Lecturer);
}

TEST_CASE("UserFactory creates HoD", "[unit][factory]") {
    auto u = UserFactory::create(Role::HoD, 3, "hod", "Thabo", 5);
    REQUIRE(u->getRole() == Role::HoD);
}

TEST_CASE("UserFactory creates Administrator", "[unit][factory]") {
    auto u = UserFactory::create(Role::Administrator, 4, "admin", "Sys Admin");
    REQUIRE(u->getRole() == Role::Administrator);
}

TEST_CASE("Polymorphism – getRole via base pointer", "[unit][polymorphism]") {
    std::unique_ptr<User> u = UserFactory::create(Role::Student, 1, "s", "S", 0, "X");
    User* base = u.get();
    REQUIRE(base->getRole() == Role::Student);
}
