PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS Department (
  department_id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS User (
  user_id INTEGER PRIMARY KEY AUTOINCREMENT,
  username TEXT UNIQUE NOT NULL,
  password_hash TEXT NOT NULL,
  full_name TEXT NOT NULL,
  role TEXT NOT NULL CHECK(role IN ('Student','Lecturer','HoD','Administrator')),
  is_active INTEGER NOT NULL DEFAULT 1,
  department_id INTEGER REFERENCES Department(department_id),
  student_number TEXT
);

CREATE TABLE IF NOT EXISTS Course (
  course_id INTEGER PRIMARY KEY AUTOINCREMENT,
  department_id INTEGER NOT NULL REFERENCES Department(department_id),
  code TEXT NOT NULL,
  title TEXT NOT NULL,
  lecturer_id INTEGER REFERENCES User(user_id)
);

CREATE TABLE IF NOT EXISTS Enrolment (
  student_id INTEGER NOT NULL REFERENCES User(user_id),
  course_id INTEGER NOT NULL REFERENCES Course(course_id),
  PRIMARY KEY (student_id, course_id)
);

CREATE TABLE IF NOT EXISTS Assignment (
  assignment_id INTEGER PRIMARY KEY AUTOINCREMENT,
  course_id INTEGER NOT NULL REFERENCES Course(course_id),
  title TEXT NOT NULL,
  description TEXT NOT NULL DEFAULT '',
  deadline TEXT NOT NULL,
  max_mark REAL NOT NULL CHECK(max_mark > 0)
);

CREATE TABLE IF NOT EXISTS Extension (
  extension_id INTEGER PRIMARY KEY AUTOINCREMENT,
  assignment_id INTEGER NOT NULL REFERENCES Assignment(assignment_id),
  student_id INTEGER REFERENCES User(user_id),
  new_deadline TEXT NOT NULL,
  reason TEXT NOT NULL,
  granted_by INTEGER NOT NULL REFERENCES User(user_id)
);

CREATE TABLE IF NOT EXISTS Submission (
  submission_id INTEGER PRIMARY KEY AUTOINCREMENT,
  assignment_id INTEGER NOT NULL REFERENCES Assignment(assignment_id),
  student_id INTEGER NOT NULL REFERENCES User(user_id),
  content TEXT NOT NULL,
  submitted_at TEXT NOT NULL,
  status TEXT NOT NULL CHECK(status IN ('Submitted','Late','Marked','Moderated','Released')),
  raw_mark REAL,
  feedback TEXT,
  final_mark REAL,
  days_late INTEGER NOT NULL DEFAULT 0,
  UNIQUE(assignment_id, student_id)
);

CREATE TABLE IF NOT EXISTS Audit_Log (
  log_id INTEGER PRIMARY KEY AUTOINCREMENT,
  submission_id INTEGER NOT NULL REFERENCES Submission(submission_id),
  changed_by INTEGER NOT NULL REFERENCES User(user_id),
  changed_at TEXT NOT NULL,
  old_mark REAL,
  new_mark REAL NOT NULL,
  reason TEXT NOT NULL
);

CREATE INDEX IF NOT EXISTS ix_submission_status ON Submission(status);
CREATE INDEX IF NOT EXISTS ix_assignment_course ON Assignment(course_id);
CREATE INDEX IF NOT EXISTS ix_enrolment_student ON Enrolment(student_id);
