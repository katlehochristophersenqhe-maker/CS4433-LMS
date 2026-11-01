# CR1 implementation record (planned v1.1)

**Status:** Implemented and verified: 65 assertions across 25 test cases pass.
The v1.1 release has not been tagged or published.

## 1. LecturerService – class-wide extension

`LecturerService::grantClassExtension` shares validation and persistence with
individual extensions, and saves an empty `studentId`, which the SQLite
repository binds as NULL.

## 2. ConsoleUI – lecturer menu

The lecturer menu has option **5. Grant class extension**. It prompts for the
assignment, new deadline, and reason, then calls `grantClassExtension`.

## 3. Student list – show extension

`StudentService::listAssignmentsWithDeadlines` provides each assignment's
original and effective deadline. The UI displays `extended_to` only when an
extension changes the deadline. The same effective-deadline helper is used when
calculating submission lateness. Individual extensions take precedence over
class-wide extensions.

## 4. Regression

`tests/test_cr1_extensions.cpp` covers class-wide persistence as NULL,
student-visible effective deadlines, submission lateness, individual-extension
precedence, and validation failures. Run the full CMake/CTest suite before
release.

## 5. Release

The user manual and draft release notes are updated. Tag and publish v1.1 only
after release approval. Startup now transactionally migrates legacy databases
whose `Extension.student_id` column is `NOT NULL`; a regression test verifies
that existing extension data is preserved.
