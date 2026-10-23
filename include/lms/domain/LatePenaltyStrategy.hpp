#pragma once
#include <memory>

namespace lms {

// Strategy pattern – late penalty calculation (Open/Closed Principle)
class LatePenaltyStrategy {
public:
    virtual ~LatePenaltyStrategy() = default;
    // Returns the final mark after applying the penalty (never below 0).
    virtual double apply(double maxMark, double rawMark, int daysLate) const = 0;
    virtual double penaltyAmount(double maxMark, int daysLate) const = 0;
};

// School rule: 10% of max mark per whole day late, up to 3 days; after that mark = 0.
class StandardLatePenaltyStrategy : public LatePenaltyStrategy {
public:
    double apply(double maxMark, double rawMark, int daysLate) const override;
    double penaltyAmount(double maxMark, int daysLate) const override;
};

} // namespace lms
