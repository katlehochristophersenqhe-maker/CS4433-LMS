# Moderated LMS – Build Guide (Phase 4)

## Requirements

- C++17 compiler (g++ 9+ or clang++)
- CMake 3.16+
- C++17 compiler only (OpenSSL no longer required)
- SQLite is **bundled** (amalgamation in `third_party/`)

## Build

```bash
cd lms
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

The executable is `build/lms`. The schema file is copied to `build/schema.sql`.

## Run

```bash
cd build
./lms            # uses ./lms.db and ./schema.sql
./lms /path/to/other.db
```

On first run the database is created and seeded with demo accounts:

| Username | Password  | Role            |
|----------|-----------|-----------------|
| admin    | admin123  | Administrator   |
| hod1     | hod123    | Head of Department |
| lec1     | lec123    | Lecturer        |
| stu1     | stu123    | Student         |

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

Passwords are stored only as **salted SHA-256 hashes** (format `saltHex$digestHex`) using a portable built-in implementation — no OpenSSL or other crypto library is required. This meets the assignment rule that passwords must never be stored in plain text. Argon2id is preferred for high-security production systems; the `IPasswordHasher` interface allows swapping the algorithm without changing services.


## Coding standard

- C++17, no compiler extensions
- Headers under `include/lms/...`, sources under `src/...`
- Exceptions for domain/persistence errors (`LmsError` hierarchy); UI catches and displays messages
- All SQL uses bound parameters (no string concatenation of user input)
