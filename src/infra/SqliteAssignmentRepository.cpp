#include "lms/infra/Repositories.hpp"

namespace lms {

SqliteAssignmentRepository::SqliteAssignmentRepository(SqliteDatabase& db) : db_(db) {}

std::optional<Assignment> SqliteAssignmentRepository::findById(int id) {
    SqliteStatement st(db_,
        "SELECT assignment_id, course_id, title, description, deadline, max_mark "
        "FROM Assignment WHERE assignment_id = ?");
    st.bind(1, id);
    if (!st.step()) return std::nullopt;
    Assignment a;
    a.id = st.columnInt(0);
    a.courseId = st.columnInt(1);
    a.title = st.columnText(2);
    a.description = st.columnText(3);
    a.deadline = st.columnText(4);
    a.maxMark = st.columnDouble(5);
    return a;
}

std::vector<Assignment> SqliteAssignmentRepository::findByCourse(int courseId) {
    SqliteStatement st(db_,
        "SELECT assignment_id, course_id, title, description, deadline, max_mark "
        "FROM Assignment WHERE course_id = ?");
    st.bind(1, courseId);
    std::vector<Assignment> out;
    while (st.step()) {
        Assignment a;
        a.id = st.columnInt(0);
        a.courseId = st.columnInt(1);
        a.title = st.columnText(2);
        a.description = st.columnText(3);
        a.deadline = st.columnText(4);
        a.maxMark = st.columnDouble(5);
        out.push_back(a);
    }
    return out;
}

std::vector<Assignment> SqliteAssignmentRepository::findForStudent(int studentId) {
    SqliteStatement st(db_,
        "SELECT a.assignment_id, a.course_id, a.title, a.description, a.deadline, a.max_mark "
        "FROM Assignment a "
        "JOIN Enrolment e ON e.course_id = a.course_id "
        "WHERE e.student_id = ?");
    st.bind(1, studentId);
    std::vector<Assignment> out;
    while (st.step()) {
        Assignment a;
        a.id = st.columnInt(0);
        a.courseId = st.columnInt(1);
        a.title = st.columnText(2);
        a.description = st.columnText(3);
        a.deadline = st.columnText(4);
        a.maxMark = st.columnDouble(5);
        out.push_back(a);
    }
    return out;
}

int SqliteAssignmentRepository::save(const Assignment& a) {
    SqliteStatement st(db_,
        "INSERT INTO Assignment (course_id, title, description, deadline, max_mark) "
        "VALUES (?, ?, ?, ?, ?)");
    st.bind(1, a.courseId).bind(2, a.title).bind(3, a.description)
      .bind(4, a.deadline).bind(5, a.maxMark);
    st.execute();
    return static_cast<int>(db_.lastInsertRowId());
}

} // namespace lms
