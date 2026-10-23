#include "lms/domain/LatePenaltyStrategy.hpp"
#include <algorithm>

namespace lms {

double StandardLatePenaltyStrategy::penaltyAmount(double maxMark, int daysLate) const {
    if (daysLate <= 0) return 0.0;
    if (daysLate > 3)  return maxMark; // full mark deducted → final = 0
    return maxMark * 0.10 * daysLate;
}

double StandardLatePenaltyStrategy::apply(double maxMark, double rawMark, int daysLate) const {
    if (daysLate > 3) return 0.0;
    const double pen = penaltyAmount(maxMark, daysLate);
    return std::max(0.0, rawMark - pen);
}

} // namespace lms
