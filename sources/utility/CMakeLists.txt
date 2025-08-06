cmake_minimum_required(VERSION 3.22)

project(ctrl1602a C)

add_executable(ctrl1602a)
target_sources(ctrl1602a
    PRIVATE
        main.c
)

install(TARGETS ctrl1602a)
