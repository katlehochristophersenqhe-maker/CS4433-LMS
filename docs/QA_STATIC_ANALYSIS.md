# Quality assurance – static analysis, metrics, code review (Phase 6)

## 1. Static analysis (cppcheck)

Install (Windows MSYS2): `pacman -S mingw-w64-ucrt-x86_64-cppcheck`
Install (Ubuntu): `sudo apt install cppcheck`

Run from project root:

```bash
cppcheck --quiet --force --check-level=exhaustive --enable=all \
  --std=c++17 --suppress=missingIncludeSystem \
  --suppress=checkersReport \
  --suppress=uninitMemberVarNoCtor:third_party/sqlite3.h \
  --suppress=unusedFunction:include/lms/domain/User.hpp \
  --suppress=unusedFunction:include/lms/domain/Submission.hpp \
  --suppress=unusedFunction:src/infra/SqliteDatabase.cpp \
  --suppress=unusedFunction:src/app/StudentService.cpp \
  -I include -I third_party \
  src 2> docs/cppcheck_report.txt
```

**Run:** Cppcheck 2.21.0, exhaustive configuration and branch checks; exit code
0 with no unsuppressed diagnostics. The SQLite amalgamation triggers
`uninitMemberVarNoCtor` warnings in its public structs, so that checker is
suppressed only for `third_party/sqlite3.h`. `unusedFunction` is suppressed only in `User.hpp`, `Submission.hpp`,
`SqliteDatabase.cpp`, and `StudentService.cpp` because these files contain
public model, database, or service APIs retained for compatibility, some of
which are not used by the current application executable.

The report is empty when there are no unsuppressed diagnostics:
[cppcheck_report.txt](./cppcheck_report.txt).

Tests are checked by the CMake/CTest workflow. The bundled Catch2-compatible
test header is excluded from this source analysis to avoid reporting its
internal implementation and macro-generated test functions.

## 2. Code metrics

Measured physical source lines in the current repository snapshot:

| Component | Lines |
|-----------|-------|
| Application source: 28 `.cpp`, `.hpp`, and `.h` files in `src/` and `include/` | 1,869 |
| Test source: five `.cpp` files; excludes `tests/catch.hpp` | 367 |

These are physical line counts and include blank lines and comments. They are
not a complexity measure.

Cyclomatic complexity focus:
- `StandardLatePenaltyStrategy::apply` – low (few branches)
- `StudentService::submit` – moderate (validation paths)
- `ConsoleUI` menus – moderate (switch-like choices)

Keep critical business methods under ~10–15 decision points where practical.

## 3. Self code-review checklist

| # | Check | Result (Y/N/Fixed) |
|---|--------|---------------------|
| 1 | Abstract User + polymorphism used for roles | |
| 2 | Repositories isolate all SQL | |
| 3 | Parameterised queries only (no string-built SQL with user input) | |
| 4 | Passwords never stored plain text | |
| 5 | Audit entry on every mark change | |
| 6 | Late penalty matches BR-01 (10%/day, max 3 days) | |
| 7 | Exceptions / validation prevent crashes on bad input | |
| 8 | No secrets committed (.gitignore has `*.db`, build/) | |
| 9 | Commit messages are meaningful | |
| 10 | README / BUILD.md enough to compile | |

Record changes made after this review in the Phase 6 report (e.g. “added null check after findByUsername”).
