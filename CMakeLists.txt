cmake_minimum_required(VERSION 3.6)
project(robol)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    commandsOutput.txt
    errorOutput.txt
    input.txt
    printingOutput.txt
    README.md
    rimalOutput.txt
    robol.c
    robol.h
    robol.l
    robol.y)

add_executable(robol ${SOURCE_FILES})