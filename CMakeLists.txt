cmake_minimum_required (VERSION 3.11)
project(Solver)

# Use gtest
include(googletest.cmake)

enable_testing()

include_directories("${PROJECT_SOURCE_DIR}/megaminx")
add_subdirectory(megaminx)
set(EXTRA_LIBS ${EXTRA_LIBS} megaminx)

include_directories("${PROJECT_SOURCE_DIR}/utils")
add_subdirectory(utils)
set(EXTRA_LIBS ${EXTRA_LIBS} utils)

include_directories("${PROJECT_SOURCE_DIR}/sqlite3")
add_subdirectory(sqlite3)
