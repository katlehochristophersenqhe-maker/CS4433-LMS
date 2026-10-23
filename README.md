# Moderated Learning Management System (LMS)

**CS4433 Phase 4 – Implementation**  
National University of Lesotho · C++17 · SQLite · CMake

## Quick start

```bash
# Compile SQLite as C, then link the application (PowerShell)
New-Item -ItemType Directory -Force build | Out-Null
gcc -c third_party/sqlite3.c -o build/sqlite3.o -DSQLITE_THREADSAFE=1 -O2
g++ -std=c++17 -I include -I third_party -O2 src/domain/*.cpp src/infra/*.cpp src/app/*.cpp src/ui/ConsoleUI.cpp src/main.cpp build/sqlite3.o -o build/lms

cp sql/schema.sql build/
cd build
./lms
```

The command above works with MinGW on Windows and does not need OpenSSL. On Linux,
append `-lpthread -ldl` to the `g++` command if your SQLite build needs them.

Or with CMake (recommended):

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
cd build && ./lms
```

## Demo accounts (seeded on first run)

| Username | Password | Role |
|----------|----------|------|
| admin | admin123 | Administrator |
| hod1 | hod123 | Head of Department |
| lec1 | lec123 | Lecturer |
| stu1 | stu123 | Student |

## Implemented (Must requirements)

- Role-based access for Administrator, Lecturer, HoD, Student  
- Abstract `User` + inheritance + `UserFactory`  
- Late-penalty **Strategy** (10 % / day, max 3 days → 0)  
- **Repository/DAO** layer; all SQL parameterised  
- Extensions (individual), resubmission before deadline  
- Marking with written feedback; marks hidden until HoD release  
- **Audit log** (who, when, old, new, reason)  
- Salted password hashes (portable SHA-256; interface ready for Argon2)  
- Exceptions + validation; menu-driven console UI  
- Offline SQLite persistence  

## Layout

```
lms/
  include/lms/{domain,app,infra,ui}/
  src/{domain,app,infra,ui}/
  sql/schema.sql
  third_party/sqlite3.{c,h}   # amalgamation
  docs/BUILD.md
  CMakeLists.txt
```

See `docs/BUILD.md` for design mapping and coding notes.
