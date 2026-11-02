# Moderated LMS – Build Guide (Phase 4)

## Requirements

- C++17 compiler (g++ 9+ or clang++)
- CMake 3.16+
- SQLite is **bundled** (amalgamation in `third_party/`)

## Build

```powershell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
Set-Location build
.\lms.exe
```

Run these commands from the repository root. The executable is `build/lms.exe`
on Windows (`build/lms` on Linux/macOS); CMake copies the schema to
`build/schema.sql`.

To run on Linux/macOS:

```bash
./build/lms
./build/lms /path/to/other.db
```

On Windows PowerShell, run `Set-Location build` and then `.\lms.exe`; use
separate commands rather than `&&` in Windows PowerShell 5.

On first run, the database is created and seeded with demo accounts:

| Username | Password  | Role            |
|----------|-----------|-----------------|
| admin    | admin123  | Administrator   |
| hod1     | hod123    | Head of Department |
| lec1     | lec123    | Lecturer        |
| stu1     | stu123    | Student         |

## Tests

From the repository root:

```powershell
cmake -S . -B build-tests -DCMAKE_BUILD_TYPE=Release
cmake --build build-tests --target lms_tests --parallel
ctest --test-dir build-tests --output-on-failure
```

The verified Windows/MinGW suite reported 29 test cases and 104 assertions.
CTest runs the single `lms_tests` target.

## Design mapping

| Design element              | Location                                      |
|----------------------------|-----------------------------------------------|
| Abstract `User` hierarchy  | `include/lms/domain/User.hpp`                 |
| Factory pattern            | `UserFactory` in `User.hpp` / `User.cpp`      |
| Strategy (late penalty)    | `LatePenaltyStrategy.hpp` / `.cpp`            |
| Repository / DAO           | `include/lms/infra/Repositories.hpp`          |
| Parameterised SQL          | all `Sqlite*Repository` classes               |
| Password hashing           | `Sha256PasswordHasher` (portable SHA-256 + salt) |
| Audit log                  | `SqliteAuditRepository` + `Audit_Log` table   |
| Layered architecture       | `domain` / `app` / `infra` / `ui` namespaces  |

## Password hashing justification

Passwords are stored as **salted SHA-256 hashes** (format `saltHex$digestHex`) using a portable built-in implementation — no OpenSSL or other crypto library is required. This avoids plaintext storage but SHA-256 is not a slow password-hashing scheme and is not suitable for production credential storage. Argon2id or another memory-hard password KDF is recommended for real deployments; the `IPasswordHasher` interface allows the algorithm to be replaced.


## Coding standard

- C++17, no compiler extensions
- Headers under `include/lms/...`, sources under `src/...`
- Exceptions for domain/persistence errors (`LmsError` hierarchy); UI catches and displays messages
- All SQL uses bound parameters (no string concatenation of user input)
