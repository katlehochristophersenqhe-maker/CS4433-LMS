#include "lms/infra/PasswordHasher.hpp"
#include "lms/domain/Types.hpp"
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <vector>

namespace lms {

namespace {
std::string toHex(const unsigned char* data, size_t len) {
    std::ostringstream oss;
    for (size_t i = 0; i < len; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
    return oss.str();
}

std::string sha256Hex(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.data()), input.size(), hash);
    return toHex(hash, SHA256_DIGEST_LENGTH);
}
} // namespace

std::string Sha256PasswordHasher::hash(const std::string& password) {
    unsigned char salt[16];
    if (RAND_bytes(salt, sizeof(salt)) != 1)
        throw PersistenceError("Failed to generate password salt");
    const std::string saltHex = toHex(salt, sizeof(salt));
    const std::string digest = sha256Hex(saltHex + password);
    return saltHex + "$" + digest;
}

bool Sha256PasswordHasher::verify(const std::string& password, const std::string& stored) {
    const auto pos = stored.find('$');
    if (pos == std::string::npos) return false;
    const std::string saltHex = stored.substr(0, pos);
    const std::string expected = stored.substr(pos + 1);
    const std::string actual = sha256Hex(saltHex + password);
    // Constant-time-ish compare
    if (actual.size() != expected.size()) return false;
    unsigned char diff = 0;
    for (size_t i = 0; i < actual.size(); ++i)
        diff |= static_cast<unsigned char>(actual[i] ^ expected[i]);
    return diff == 0;
}

} // namespace lms
