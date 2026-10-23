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

// Portable SHA-256 + random salt (no external crypto library).
// Stored format: "saltHex$hashHex".
// Justification: works offline on Windows/Linux/macOS with only a C++17 compiler.
// Production systems should prefer Argon2id (e.g. libsodium) for memory-hard hashing.
// IPasswordHasher allows swapping the algorithm without changing services.
class Sha256PasswordHasher : public IPasswordHasher {
public:
    std::string hash(const std::string& password) override;
    bool verify(const std::string& password, const std::string& stored) override;
};

} // namespace lms
