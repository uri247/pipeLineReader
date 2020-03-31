#include <iostream>
#include <string>
#include <system_error>
#include <unistd.h>
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
    m_parent_to_child_pipe.pipe();
    m_child_to_parent_pipe.pipe();

    int child_id = fork();
    std::cout << "Forked. Child ID: " << child_id << std::endl;

    // sleep( 30 );

    if( child_id < 0 ) {
        throw std::system_error();
    }
    else if( child_id == 0 ) {
        // This is the child

        // close un necessary ends of the pipes
        m_parent_to_child_pipe.close( Pipe::write_end );
        m_child_to_parent_pipe.close( Pipe::read_end );

        // redirect stdin
        result = dup2( m_parent_to_child_pipe.read_pipe(), STDIN_FILENO );
        if( result < 0 ) {
            throw std::system_error();
        }
        // redirect stdout and stderr
        result = dup2( m_child_to_parent_pipe.write_pipe(), STDOUT_FILENO );
        if( result < 0 ) {
            throw std::system_error();
        }
        result = dup2( m_child_to_parent_pipe.write_pipe(), STDERR_FILENO );
        if( result < 0 ) {
            throw std::system_error();
        }

        // Once duplicated, close the duplicated ends of the pipes
        m_parent_to_child_pipe.close( Pipe::read_end );
        m_child_to_parent_pipe.close( Pipe::write_end );

        // Child process is now running, and stdin/stdout/stderr correctly redirected. Exec the process
        // we want

        const char* argv[] = {"shell", "-c", cmd.c_str(), nullptr};
        execv( "/bin/sh", const_cast<char**>(argv));
        throw std::system_error();
    }
    else {
        // This is the parent

        // close unused ends of pipes
        m_parent_to_child_pipe.close( Pipe::read_end );
        m_child_to_parent_pipe.close( Pipe::write_end );
    }
}

int PipedProcess::write(const std::string& msg)
{
    int wrote = ::write( m_parent_to_child_pipe.write_pipe(), msg.c_str(), msg.length());
    return wrote;

}

int PipedProcess::read(char* buffer, int size)
{
    int red = ::read( m_child_to_parent_pipe.read_pipe(), buffer, size);
    return red;
}

