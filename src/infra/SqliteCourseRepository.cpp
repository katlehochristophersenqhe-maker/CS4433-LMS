#include "lms/infra/Repositories.hpp"

namespace lms {

SqliteCourseRepository::SqliteCourseRepository(SqliteDatabase& db) : db_(db) {}

std::optional<Course> SqliteCourseRepository::findById(int id) {
    SqliteStatement st(db_,
        "SELECT course_id, department_id, code, title, lecturer_id FROM Course WHERE course_id = ?");
    st.bind(1, id);
    if (!st.step()) return std::nullopt;
    Course c;
    c.id = st.columnInt(0);
    c.departmentId = st.columnInt(1);
    c.code = st.columnText(2);
    c.title = st.columnText(3);
    if (!st.columnIsNull(4)) c.lecturerId = st.columnInt(4);
    return c;
}

std::vector<Course> SqliteCourseRepository::findByDepartment(int departmentId) {
    SqliteStatement st(db_,
        "SELECT course_id, department_id, code, title, lecturer_id FROM Course WHERE department_id = ?");
    st.bind(1, departmentId);
    std::vector<Course> out;
    while (st.step()) {
        Course c;
        c.id = st.columnInt(0);
        c.departmentId = st.columnInt(1);
        c.code = st.columnText(2);
        c.title = st.columnText(3);
        if (!st.columnIsNull(4)) c.lecturerId = st.columnInt(4);
        out.push_back(c);
    }
    return out;
}

int SqliteCourseRepository::save(const Course& c) {
    SqliteStatement st(db_,
        "INSERT INTO Course (department_id, code, title, lecturer_id) VALUES (?, ?, ?, ?)");
    st.bind(1, c.departmentId).bind(2, c.code).bind(3, c.title);
    if (c.lecturerId) st.bind(4, *c.lecturerId); else st.bindNull(4);
    st.execute();
    return static_cast<int>(db_.lastInsertRowId());
}

void SqliteCourseRepository::enrol(int studentId, int courseId) {
    SqliteStatement st(db_,
        "INSERT OR IGNORE INTO Enrolment (student_id, course_id) VALUES (?, ?)");
    st.bind(1, studentId).bind(2, courseId);
    st.execute();
}

bool SqliteCourseRepository::isEnrolled(int studentId, int courseId) {
    SqliteStatement st(db_,
        "SELECT 1 FROM Enrolment WHERE student_id = ? AND course_id = ?");
    st.bind(1, studentId).bind(2, courseId);
    return st.step();
}

int SqliteCourseRepository::saveDepartment(const std::string& name) {
    SqliteStatement st(db_, "INSERT INTO Department (name) VALUES (?)");
    st.bind(1, name);
    st.execute();
    return static_cast<int>(db_.lastInsertRowId());
}

} // namespace lms
