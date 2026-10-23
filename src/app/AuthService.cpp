#include "lms/app/Services.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>

namespace lms {

std::string nowIso() {
    std::time_t t = std::time(nullptr);
    std::tm tm{};
#if defined(_WIN32)
    gmtime_s(&tm, &t);
#else
    gmtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

// Simple day difference using date portion only (school offline use).
int daysLate(const std::string& deadlineIso, const std::string& submittedIso) {
    if (submittedIso <= deadlineIso) return 0;
    // Parse YYYY-MM-DD
    auto toDays = [](const std::string& iso) -> long {
        if (iso.size() < 10) return 0;
        int y = std::stoi(iso.substr(0, 4));
        int m = std::stoi(iso.substr(5, 2));
        int d = std::stoi(iso.substr(8, 2));
        // Approximate day number
        return static_cast<long>(y) * 365 + m * 30 + d;
    };
    long diff = toDays(submittedIso) - toDays(deadlineIso);
    return diff > 0 ? static_cast<int>(diff) : 0;
}

AuthService::AuthService(IUserRepository& users, IPasswordHasher& hasher)
    : users_(users), hasher_(hasher) {}

std::unique_ptr<User> AuthService::login(const std::string& username, const std::string& password) {
    auto user = users_.findByUsername(username);
    if (!user) throw AuthError("Invalid username or password");
    if (!user->isActive()) throw AuthError("Account is deactivated");
    const std::string stored = users_.passwordHash(user->id());
    if (!hasher_.verify(password, stored)) throw AuthError("Invalid username or password");
    return user;
}

} // namespace lms
