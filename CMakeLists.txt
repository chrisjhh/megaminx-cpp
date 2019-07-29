cmake_minimum_required (VERSION 2.6)
project(Solver)

include_directories("${PROJECT_SOURCE_DIR}/megaminx")
add_subdirectory(megaminx)
set(EXTRA_LIBS ${EXTRA_LIBS} megaminx)

include_directories("${PROJECT_SOURCE_DIR}/utils")
add_subdirectory(utils)
set(EXTRA_LIBS ${EXTRA_LIBS} utils)

include_directories("${PROJECT_SOURCE_DIR}/sqlite3")
add_subdirectory(sqlite3)
