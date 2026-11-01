# Release Notes – Moderated LMS v1.1

**Status:** Draft — implementation is complete, but v1.1 has not been tagged or published.
**Change request:** CR1

## What is new

- Lecturers can grant a **class-wide** deadline extension (whole class, not only one student).
- Students see the **effective / extended deadline** on their assignment list when an extension applies.
- Startup upgrades legacy databases whose `Extension.student_id` column is `NOT NULL`, preserving existing extension rows.

## What did not change

- Individual extensions still work.
- Late penalty rules unchanged.
- Marking, moderation, release, audit log unchanged.

## Upgrade

Back up `lms.db` before upgrading. On startup, the application checks the
`Extension.student_id` constraint and transactionally rebuilds the table if
the column is `NOT NULL`. Existing extension rows and IDs are preserved. The
migration can safely run more than once.
