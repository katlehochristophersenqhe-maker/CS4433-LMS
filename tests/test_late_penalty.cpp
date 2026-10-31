#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "lms/domain/LatePenaltyStrategy.hpp"

using namespace lms;

TEST_CASE("Late penalty – on time (0 days)", "[unit][penalty][BVA]") {
    StandardLatePenaltyStrategy s;
    REQUIRE(s.penaltyAmount(100.0, 0) == Approx(0.0));
    REQUIRE(s.apply(100.0, 80.0, 0) == Approx(80.0));
}

TEST_CASE("Late penalty – negative days treated as on time", "[unit][penalty][EP]") {
    StandardLatePenaltyStrategy s;
    REQUIRE(s.penaltyAmount(100.0, -1) == Approx(0.0));
    REQUIRE(s.apply(100.0, 90.0, -5) == Approx(90.0));
}

TEST_CASE("Late penalty – 1 day late (boundary)", "[unit][penalty][BVA]") {
    StandardLatePenaltyStrategy s;
    // 10% of 100 = 10
    REQUIRE(s.penaltyAmount(100.0, 1) == Approx(10.0));
    REQUIRE(s.apply(100.0, 80.0, 1) == Approx(70.0));
}

TEST_CASE("Late penalty – 2 days late", "[unit][penalty][EP]") {
    StandardLatePenaltyStrategy s;
    REQUIRE(s.penaltyAmount(100.0, 2) == Approx(20.0));
    REQUIRE(s.apply(100.0, 80.0, 2) == Approx(60.0));
}

TEST_CASE("Late penalty – 3 days late (upper boundary of scale)", "[unit][penalty][BVA]") {
    StandardLatePenaltyStrategy s;
    REQUIRE(s.penaltyAmount(100.0, 3) == Approx(30.0));
    REQUIRE(s.apply(100.0, 80.0, 3) == Approx(50.0));
}

TEST_CASE("Late penalty – 4 days late (just beyond 3 → mark zero)", "[unit][penalty][BVA]") {
    StandardLatePenaltyStrategy s;
    REQUIRE(s.penaltyAmount(100.0, 4) == Approx(100.0));
    REQUIRE(s.apply(100.0, 80.0, 4) == Approx(0.0));
}

TEST_CASE("Late penalty – many days late", "[unit][penalty][EP]") {
    StandardLatePenaltyStrategy s;
    REQUIRE(s.apply(100.0, 100.0, 10) == Approx(0.0));
}

TEST_CASE("Late penalty – never returns negative final mark", "[unit][penalty]") {
    StandardLatePenaltyStrategy s;
    // raw mark smaller than penalty
    REQUIRE(s.apply(100.0, 5.0, 2) == Approx(0.0));
}

TEST_CASE("Late penalty – different max mark (50)", "[unit][penalty][EP]") {
    StandardLatePenaltyStrategy s;
    // 10% of 50 = 5 per day
    REQUIRE(s.penaltyAmount(50.0, 1) == Approx(5.0));
    REQUIRE(s.apply(50.0, 40.0, 1) == Approx(35.0));
    REQUIRE(s.apply(50.0, 40.0, 4) == Approx(0.0));
}
