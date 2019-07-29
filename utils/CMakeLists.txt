add_library(utils DBCache.cpp)

target_link_libraries(utils sqlite3)
include_directories("${PROJECT_SOURCE_DIR}/sqlite3")


#----- tests
include(CTest)

add_executable(t_sqlite utest/t_sqlite.cpp)
target_link_libraries(t_sqlite gtest_main sqlite3)

add_test(SQLITE_Tests t_sqlite)
