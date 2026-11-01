# Release Notes – Moderated LMS v1.0

**Status:** Draft — v1.0 has not been tagged or published.
**Repository:** https://github.com/katlehochristophersenqhe-maker/CS4433-LMS

## Summary

First production release of the Moderated Learning Management System for offline use in a secondary school. Students submit work; lecturers mark and give feedback; the Head of Department moderates and releases marks; an administrator manages users, departments and courses.

## Features

- Role-based access: Student, Lecturer, HoD, Administrator
- Assignment submission and resubmission before the deadline
- Automatic late penalty (10% of max mark per day, maximum 3 days; thereafter mark is zero)
- Individual deadline extensions by lecturers
- Marking with written feedback; marks hidden until HoD release
- Moderation with mandatory reason when a mark is changed
- Audit log of every mark change (who, when, why)
- SQLite persistence; salted password hashes (no plain-text passwords)
- Console menu interface (no internet required)

## System requirements

- Windows 10/11, Linux or macOS
- C++17 compiler (MinGW, MSYS2, or GCC/Clang)
- About 50 MB disk space for source and database

## How to install

See **docs/INSTALLATION.md**.

## Known limitations

- Console UI only (no web or GUI client)
- Single SQLite file per installation (no multi-server cluster)
- Late penalty uses whole calendar days from ISO timestamps

## Upgrade path

Future changes (e.g. class-wide extensions in v1.1) will be tagged separately. Backup `lms.db` before upgrading.
