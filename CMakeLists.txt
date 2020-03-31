CMAKE_MINIMUM_REQUIRED(VERSION 3.6)
PROJECT(pipeLineReader)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_REV 0)


add_executable(parent
    parent.h parent.cpp pipe.h pipe.cpp)

add_executable(execute exec.cpp)