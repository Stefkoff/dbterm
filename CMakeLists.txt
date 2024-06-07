cmake_minimum_required(VERSION 3.27)
project(dbterm C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CURSES_NEED_WIDE true)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/include)

pkg_check_modules(LIBMYSQLCLIENT REQUIRED mysqlclient)

foreach(FLAG ${LIBMYSQLCLIENT_CFLAGS_OTHER})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG}")
endforeach()

link_directories(${LIBMYSQLCLIENT_LIBRARY_DIRS})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/help.txt
        ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

add_subdirectory(${PROJECT_SOURCE_DIR}/src)
target_include_directories(dbterm PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})
target_link_libraries(dbterm ${CURSES_LIBRARIES} ${LIBMYSQLCLIENT_LIBRARIES} m)
