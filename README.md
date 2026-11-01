# Moderated Learning Management System (LMS)

**CS4433 Phase 4 – Implementation**  
National University of Lesotho · C++17 · SQLite · CMake

## Quick start

```powershell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
Set-Location build
.\lms.exe
```

For Linux or macOS, run the same CMake commands and launch `./build/lms`.

For a fresh presentation database, run this from the project root after
building:

```powershell
.\build\lms.exe .\build\presentation-demo-v2.db
```

The database contains six submissions across three assignments, including
submitted work, a submission awaiting moderation, and released marks, so the
lecturer submission list has several students and statuses to browse. The app
does not overwrite or automatically update an existing database; use a new
filename to get the updated sample data.

## Demo accounts (seeded on first run)

| Username | Password | Role |
|----------|----------|------|
| admin | admin123 | Administrator |
| hod1 | hod123 | Head of Department |
| lec1 | lec123 | Lecturer |
| stu1 | stu123 | Student |
| stu2 | stu123 | Student |
| stu3 | stu123 | Student |

## Implemented (Must requirements)

- Role-based access for Administrator, Lecturer, HoD, Student  
- Abstract `User` + inheritance + `UserFactory`  
- Late-penalty **Strategy** (10 % / day, max 3 days → 0)  
- **Repository/DAO** layer; all SQL parameterised  
- Individual and class-wide deadline extensions; individual extensions take
  precedence, and students see the effective deadline
- Marking with written feedback; marks hidden until HoD release  
- Lecturer submission browsing and HoD access to released marks
- **Audit log** (who, when, old, new, reason)  
- Salted password hashes (portable SHA-256; replace with a memory-hard KDF for
  production credential storage)
- Exceptions + validation; menu-driven console UI  
- Offline SQLite persistence  

## Layout

The repository root contains `include/`, `src/`, `sql/`, `third_party/`,
`tests/`, `docs/`, and `CMakeLists.txt`.

See `docs/BUILD.md` for design mapping and coding notes.

## Phase 5 tests

The Phase 5 tests are in `tests/`. The test target is defined in
`CMakeLists.txt`.

### Run with CMake

```powershell
cmake -S . -B build-tests
cmake --build build-tests --target lms_tests
ctest --test-dir build-tests --output-on-failure
```

The verified Windows/MinGW run reported 26 test cases and 74 assertions.
CTest runs the `lms_tests` target. See `docs/BUILD.md` for the full build and
test instructions.
