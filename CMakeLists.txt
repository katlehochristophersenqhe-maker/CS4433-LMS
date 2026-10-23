cmake_minimum_required(VERSION 3.16)
project(ModeratedLMS VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Warnings
if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(OpenSSL REQUIRED)

# SQLite amalgamation (bundled)
add_library(sqlite3 STATIC third_party/sqlite3.c)
target_include_directories(sqlite3 PUBLIC third_party)
target_compile_definitions(sqlite3 PRIVATE SQLITE_THREADSAFE=1 SQLITE_ENABLE_RTREE=0)

# Domain library
add_library(lms_domain
  src/domain/User.cpp
  src/domain/LatePenaltyStrategy.cpp
  src/domain/Submission.cpp
  src/domain/Assignment.cpp
)
target_include_directories(lms_domain PUBLIC include)

# Infrastructure library
add_library(lms_infra
  src/infra/SqliteDatabase.cpp
  src/infra/PasswordHasher.cpp
  src/infra/SqliteUserRepository.cpp
  src/infra/SqliteSubmissionRepository.cpp
  src/infra/SqliteAssignmentRepository.cpp
  src/infra/SqliteExtensionRepository.cpp
  src/infra/SqliteAuditRepository.cpp
  src/infra/SqliteCourseRepository.cpp
)
target_include_directories(lms_infra PUBLIC include third_party)
target_link_libraries(lms_infra PUBLIC lms_domain sqlite3 OpenSSL::SSL OpenSSL::Crypto)

# Application services
add_library(lms_app
  src/app/AuthService.cpp
  src/app/StudentService.cpp
  src/app/LecturerService.cpp
  src/app/HoDService.cpp
  src/app/AdminService.cpp
)
target_include_directories(lms_app PUBLIC include)
target_link_libraries(lms_app PUBLIC lms_infra lms_domain)

# Main executable
add_executable(lms src/main.cpp src/ui/ConsoleUI.cpp)
target_include_directories(lms PRIVATE include)
target_link_libraries(lms PRIVATE lms_app lms_infra lms_domain)

# Install schema path relative to binary for convenience
configure_file(${CMAKE_SOURCE_DIR}/sql/schema.sql ${CMAKE_BINARY_DIR}/schema.sql COPYONLY)

message(STATUS "Moderated LMS configured. Build with: cmake --build .")
