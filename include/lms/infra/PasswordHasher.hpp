#pragma once
#include <string>

namespace lms {

// Interface so the hashing algorithm can be swapped (DIP).
class IPasswordHasher {
public:
    virtual ~IPasswordHasher() = default;
    virtual std::string hash(const std::string& password) = 0;
    virtual bool verify(const std::string& password, const std::string& stored) = 0;
};

// OpenSSL SHA-256 + random salt. Stored format: "saltHex$hashHex".
// Justification: OpenSSL is widely available and sufficient for a school offline system.
// Production systems should prefer Argon2id (libsodium) for memory-hard hashing.
class Sha256PasswordHasher : public IPasswordHasher {
public:
    std::string hash(const std::string& password) override;
    bool verify(const std::string& password, const std::string& stored) override;
};

} // namespace lms
