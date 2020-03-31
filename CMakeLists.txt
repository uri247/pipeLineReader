CMAKE_MINIMUM_REQUIRED(VERSION 3.6)
PROJECT(pipeLineReader)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_REV 0)
set(quotes_url
        https://gist.githubusercontent.com/erickedji/68802/raw/7264f2d232702b4013490a0b2f9286cfa1b817e3/quotes.txt)

add_executable(parent
    parent.h parent.cpp pipe.h pipe.cpp)

add_executable(execute exec.cpp)

add_executable(child child.cpp)

add_custom_command(
    OUTPUT quotes.txt
    COMMAND curl --fail --location -o quotes.txt ${quotes_url}
    COMMENT "Bring over quotes.txt file"
)
add_custom_target(quotes-file DEPENDS quotes.txt)
add_dependencies(parent quotes-file)
