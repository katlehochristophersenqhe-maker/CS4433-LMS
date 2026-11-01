# Release Notes â€“ Moderated LMS v1.1

**Status:** Published
**Change request:** CR1

The `v1.1` tag points to commit
`81afa92567b43348c17821092a37c328f0df8c2c`. The Windows x64 release archive is
available from the [GitHub release page](https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/releases/tag/v1.1).

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

Changes made on `main` after the `v1.1` tag, including lecturer-wide submission
browsing and expanded presentation sample data, are not included in this
release.
