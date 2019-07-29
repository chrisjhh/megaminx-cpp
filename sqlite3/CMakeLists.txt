
# Download the souce and headers if they are not present
# This saves us from duplicating the sqlite3 source in our own repo!
if(EXISTS "${CMAKE_SOURCE_DIR}/sqlite3/sqlite3.c")
  # Nothing to do
else()
  # Download the amalgamation zip
  file(DOWNLOAD "https://www.sqlite.org/2019/sqlite-amalgamation-3290000.zip" "${CMAKE_BINARY_DIR}/sqlite3.zip")
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E  tar xzf "${CMAKE_BINARY_DIR}/sqlite3.zip"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
  file(REMOVE "${CMAKE_BINARY_DIR}/sqlite3.zip")
  file(COPY "${CMAKE_BINARY_DIR}/sqlite-amalgamation-3290000/sqlite3.h" DESTINATION "${CMAKE_SOURCE_DIR}/sqlite3")
  file(COPY "${CMAKE_BINARY_DIR}/sqlite-amalgamation-3290000/sqlite3ext.h" DESTINATION "${CMAKE_SOURCE_DIR}/sqlite3")
  file(COPY "${CMAKE_BINARY_DIR}/sqlite-amalgamation-3290000/sqlite3.c" DESTINATION "${CMAKE_SOURCE_DIR}/sqlite3")
  file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/sqlite-amalgamation-3290000")
endif()

add_library(sqlite3 sqlite3.c)
