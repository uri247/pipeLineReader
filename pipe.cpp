#include <iostream>
#include <string>
#include <system_error>
#include <unistd.h>
#include <poll.h>
#include "pipe.h"
#include "fcntl.h"


void Pipe::pipe() {
    int result = ::pipe( _value );
    if( result < 0 ) {
        throw std::system_error();
    }

}

void Pipe::close( end_type end ) {
    int& fd = _value[end];

    if( fd != -1 ) {
        ::close(fd);
        fd = -1;
    }
}


// --------------------------------- PipedProcess ---------------------------------

void PipedProcess::command(const std::string& cmd) {
    const char* argv[] = {"shell", "-c", cmd.c_str(), nullptr};
    start("/bin/sh", argv );
}

void PipedProcess::start(const std::string& path, const char *const argv[])
{
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
        result = dup2(m_parent_to_child_pipe.read_fd(), STDIN_FILENO );
        if( result < 0 ) {
            throw std::system_error();
        }
        // redirect stdout and stderr
        result = dup2(m_child_to_parent_pipe.write_fd(), STDOUT_FILENO );
        if( result < 0 ) {
            throw std::system_error();
        }
        result = dup2(m_child_to_parent_pipe.write_fd(), STDERR_FILENO );
        if( result < 0 ) {
            throw std::system_error();
        }

        // Once duplicated, close the duplicated ends of the pipes
        m_parent_to_child_pipe.close( Pipe::read_end );
        m_child_to_parent_pipe.close( Pipe::write_end );

        // Child process is now running, and stdin/stdout/stderr correctly redirected. Exec the process
        // we want

        //const char* argv[] = {"shell", "-c", cmd.c_str(), nullptr};
        //execv( "/bin/sh", const_cast<char**>(argv));
        execv(path.c_str(), const_cast<char**>(argv));
        throw std::system_error();
    }
    else {
        // This is the parent

        // close unused ends of pipes
        m_parent_to_child_pipe.close( Pipe::read_end );
        m_child_to_parent_pipe.close( Pipe::write_end );

        fd_set set;

        // change the reading pipe to async
        int flags = fcntl(m_child_to_parent_pipe.read_fd(), F_GETFL);
        flags = O_NONBLOCK;
        fcntl (m_child_to_parent_pipe.read_fd(), F_SETFL, flags );
    }
}


int PipedProcess::write(const std::string& msg)
{
    int wrote = ::write(m_parent_to_child_pipe.write_fd(), msg.c_str(), msg.length());
    return wrote;
}

int PipedProcess::read(char* buffer, int size)
{
    int timeout = 30;
    int red;
    int tries = 0;

    pollfd fd = {m_child_to_parent_pipe.read_fd(), POLLIN, 0 };
    poll( &fd, 1, timeout * 1000 );
    red = ::read( m_child_to_parent_pipe.read_fd(), buffer, size );
    return red;

//    while( true ) {
//        red = ::read(m_child_to_parent_pipe.read_fd(), buffer, size);
//        if( red == -1 && errno == EAGAIN ) {
//            ++tries;
//            if( tries == timeout )
//                throw timeout_exception("time out reading from pipe");
//            sleep( 1 );
//        }
//        else {
//            break;
//        }
//    }
//    return red;
}
