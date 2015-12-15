cmake_minimum_required(VERSION 2.8)

project(budgit)


set(SOURCE_FILES
    account.c
    accountfile.c  
    graphics.c 
    interface.c  
    program.c  
    tag.c
    )

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(budgit ${SOURCE_FILES} main.c)
target_link_libraries(budgit ${CURSES_LIBRARIES})


