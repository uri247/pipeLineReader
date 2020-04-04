#include <iostream>
#include <string>
#include <system_error>
#include <cassert>
#include <unistd.h>
#include "pipe.h"
#include "fcntl.h"


void Pipe::pipe() {
    int result = ::pipe( _value );
    if( result < 0 ) {
        throw std::system_error();
    }

}

void Pipe::close( int& fd ) {
    assert(&fd == &_value[0] || &fd == &_value[1]);
    ::close(fd);
    fd = -1;
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
        m_parent_to_child_pipe.close( m_parent_to_child_pipe.write_end() );
        m_child_to_parent_pipe.close( m_child_to_parent_pipe.read_end() );

        // redirect stdin
        result = dup2( m_parent_to_child_pipe.read_end(), STDIN_FILENO );
        if( result < 0 ) {
            throw std::system_error();
        }
        // redirect stdout and stderr
        result = dup2( m_child_to_parent_pipe.write_end(), STDOUT_FILENO );
        if( result < 0 ) {
            throw std::system_error();
        }
        result = dup2( m_child_to_parent_pipe.write_end(), STDERR_FILENO );
        if( result < 0 ) {
            throw std::system_error();
        }

        // Once duplicated, close the duplicated ends of the pipes
        m_parent_to_child_pipe.close( m_parent_to_child_pipe.read_end() );
        m_child_to_parent_pipe.close( m_child_to_parent_pipe.write_end() );

        // Child process is now running, and stdin/stdout/stderr correctly redirected. Exec the process
        // we want

        const char* argv[] = {"shell", "-c", cmd.c_str(), nullptr};
        execv( "/bin/sh", const_cast<char**>(argv));
        throw std::system_error();
    }
    else {
        // This is the parent

        // close unused ends of pipes
        m_parent_to_child_pipe.close( m_parent_to_child_pipe.read_end() );
        m_child_to_parent_pipe.close( m_child_to_parent_pipe.write_end() );

        // change the reading pipe to async
        int flags = fcntl( m_child_to_parent_pipe.read_end(), F_GETFL);
        flags = O_NONBLOCK;
        fcntl ( m_child_to_parent_pipe.read_end(), F_SETFL, flags );
    }
}


int PipedProcess::write(const std::string& msg)
{
    int wrote = ::write( m_parent_to_child_pipe.write_end(), msg.c_str(), msg.length());
    return wrote;
}

int PipedProcess::read(char* buffer, int size)
{
    int timeout = 30;
    int red;
    int tries = 0;

    while( true ) {
        red = ::read( m_child_to_parent_pipe.read_end(), buffer, size);
        if( red == -1 && errno == EAGAIN ) {
            ++tries;
            if( tries == timeout )
                throw timeout_exception("time out reading from pipe");
        }
        else {
            break;
        }
    }
    return red;
}

