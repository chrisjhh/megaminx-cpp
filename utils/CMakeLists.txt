add_library(utils DBCache.cpp)

find_package(Sqlite3 PATHS "C:\Progam Files (x86)")
target_link_libraries(utils sqlite3::sqlite3)

add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build)

#----- tests
include(CTest)

add_executable(t_sqlite utest/t_sqlite.cpp)
target_link_libraries(t_sqlite gtest_main sqlite3::sqlite3)