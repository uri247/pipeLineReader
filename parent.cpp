#include <iostream>
#include <string>
#include <stdexcept>
#include <system_error>
#include <cstring>
#include <unistd.h>
#include "parent.h"


int main()
{
    try {
        Application app{};
        app.main( );
    }
    catch( const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }
}

void FdPair::pipe() {
    int result = ::pipe( _value );
    if( result < 0 ) {
        throw std::system_error();
    }

}

int Application::main( )
{
    FdPair stdin_pipes;
    FdPair stdout_pipes;
    int result;

    stdin_pipes.pipe();
    stdout_pipes.pipe();

    int child_num = fork();

    std::cout << "post fork, child_num: " << child_num << std::endl;

    if( child_num < 0 ) {
        throw std::system_error();
    }
    else if( child_num == 0) {
        // This is the child
        std::cout << "Child is speaking" << std::endl;

        // redirect stdin
        result = dup2( stdin_pipes.read_pipe(), STDIN_FILENO);
        if( result < 0 ) {
            throw std::system_error();
        }

        // redirect stdout and stderr
        result = dup2( stdout_pipes.write_pipe(), STDOUT_FILENO);
        if( result < 0 ) {
            throw std::system_error();
        }
        result = dup2( stdout_pipes.write_pipe(), STDERR_FILENO);
        if( result < 0 ) {
            throw std::system_error();
        }

        // everything was either redirected or isn't used
        stdin_pipes.close_both();
        stdout_pipes.close_both();

        //execl("/usr/bin/bash", "/usr/bin/bash", nullptr);
        std::string line;
        std::cin >> line;
        std::cout << "something was received" << std::endl;
    }
    else {
        // this is the parent
        std::cout << "Parent is speaking" << std::endl;

        // close unused file descriptors
        stdin_pipes.close(FdPair::read_end);
        stdout_pipes.close( FdPair::write_end );

        char msg[] = "ls\n";
        int wrote = write(stdin_pipes.write_pipe(), msg, std::strlen(msg));
        std::cout << "wrote " << wrote << " characters" << std::endl;

        char ch;
        while(read(stdout_pipes.read_pipe(), &ch, 1)) {
            std::cout << ch;
        }

    }
}
