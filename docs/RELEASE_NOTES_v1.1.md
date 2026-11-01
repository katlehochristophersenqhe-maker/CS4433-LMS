# Release Notes – Moderated LMS v1.1

**Status:** Draft — implementation is complete, but v1.1 has not been tagged or published.
**Change request:** CR1

## What is new

- Lecturers can grant a class-wide deadline extension with a reason.
- Students see the original and effective extended deadline on their assignment list.
- Submissions use the same effective deadline shown to students; individual
  extensions take precedence over class-wide extensions.
- Startup migrates older `Extension` tables that require a student ID to allow
  class-wide rows, preserving existing extensions.

## What did not change

- Individual extensions remain supported.
- Late-penalty rules, marking, moderation, mark release, and audit behavior are
  unchanged.

## Upgrade

Back up `lms.db` before upgrading. On startup, the application checks the
`Extension.student_id` constraint and rebuilds the table transactionally if it
is `NOT NULL`. Existing extension rows and IDs are preserved. The migration is
idempotent. Verify the backup before using it in production.
