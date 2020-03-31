#include <iostream>
#include <string>
#include <system_error>
#include <memory>
#include <unistd.h>
#include <paths.h>
#include "pipe.h"


void Pipe::pipe() {
    int result = ::pipe( _value );
    if( result < 0 ) {
        throw std::system_error();
    }

}

void Pipe::close(end_type end) {
    ::close(_value[end]);
    _value[end] = -1;
}



// --------------------------------- PipedProcess ---------------------------------

void PipedProcess::start(const std::string& cmd) {
    int result;
    m_stdin_pipe.pipe();
    m_stdout_pipe.pipe();

    int child_id = fork();
    std::cout << "Forked. Child ID: " << child_id << std::endl;

//    std::cout << "Sleeping 30 ... " << std::flush;
//    sleep( 12 );
//    std::cout << "done" << std::endl;


    if( child_id < 0 ) {
        throw std::system_error();
    } else if( child_id == 0 ) {
        // This is the child
        std::cout << "Child is speaking" << std::endl;

        // close un necessary ends of the pipes
        m_stdin_pipe.close( Pipe::write_end );
        m_stdout_pipe.close( Pipe::read_end );

        // redirect stdin
        result = dup2( m_stdin_pipe.read_pipe(), STDIN_FILENO );
        if( result < 0 ) {
            throw std::system_error();
        }
        // redirect stdout and stderr
        result = dup2( m_stdout_pipe.write_pipe(), STDOUT_FILENO );
        if( result < 0 ) {
            throw std::system_error();
        }
        result = dup2( m_stdout_pipe.write_pipe(), STDERR_FILENO );
        if( result < 0 ) {
            throw std::system_error();
        }

        // Once duplicated, close the duplicated ends of the pipes
        m_stdin_pipe.close( Pipe::read_end );
        m_stdout_pipe.close( Pipe::write_end );

        // Child process is now running, and stdin/stdout/stderr correctly redirected. Exec the process
        // we want

//        char a1[] = "sh";
//        char a2[] = "-c";
//        //auto a3 = std::unique_ptr<char[]>{ new char[200]};
//        auto a3 = std::make_unique<char[]>( 200 );
//        char* argv[] = {a1, a2, a3.get(), nullptr};
        //execl("/bin/bash", "/bin/bash", nullptr);
        //execl("/bin/bash", "prog", "-c", "ls", nullptr);
        execl("/bin/bash", "prog", "-c", "ls", nullptr);

        throw std::system_error();
        //execv( _PATH_BSHELL, argv );
        //exit( 127 );
    } else {
        std::cout << "Parent is speaking" << std::endl;

        // close unused ends of pipes
        m_stdin_pipe.close( Pipe::read_end );
        m_stdout_pipe.close( Pipe::write_end );
    }
}


int PipedProcess::write(std::string msg)
{
    int wrote = ::write( m_stdin_pipe.write_pipe(), msg.c_str(), msg.length());
    return wrote;

}

int PipedProcess::read(char* buffer, int size)
{
    int red = ::read(m_stdout_pipe.read_pipe(), buffer, size);
    return red;
}

