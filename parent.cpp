#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <unistd.h>
#include "parent.h"
#include "pipe.h"
#pragma ide diagnostic ignored "MemberFunctionCanBeStatic"

#define cpp_context


int main()
{
    try {
        Application app{};
        app.main( );
    }
    catch( const std::exception& ex) {
        pid_t pid = getpid();
        std::cerr << "Exception on " << pid << ": " << ex.what() << std::endl;
    }
}


Application::Application() = default;


int Application::main( )
{
    play_start();
    play_command();
    play_bash();
    play_child();
    play_timeout();
    return 0;
}


std::string Application::read_to_delim( PipedProcess& process, char delim)
{
    std::ostringstream ss;

    while(true) {
        char buff[1020];
        int red = process.read(buff, 1000);
        if( red == 0 )
            break;
        ss.write(buff, red);
        if( buff[red - 1] == delim )
            break;
    }
    return ss.str();
}


std::string Application::get_msg(PipedProcess& process) {
    std::string msg = read_to_delim(process, '\0' );
    msg.pop_back();
    return msg;
}

void Application::play_child( )
{
    PipedProcess process;
    char buff[1020];
    std::string msg;
    const char* const argv [] = { nullptr, nullptr };

    process.start( std::string("./child"), argv );

    msg = get_msg( process );
    std::cout << msg << std::endl;

    for( int i=0; i<10000000; ++i )
    // for( int i=0; i<200; ++i )
    {
        process.write( std::string("RECORD\n"));
        msg = get_msg( process );
        std::cout << msg << std::endl;
        if( msg.length() == 0 )
            break;
    }
    process.write( std::string( "QUIT\n"));
}

void Application::play_start()
{
    const std::string path("/usr/bin/lsb_release");
    const char* const argv [] = { "lsb_release", "-i", "-c", "-r", "-d", nullptr };
    PipedProcess process;
    process.start(path, argv);
    std::string output = read_to_delim( process, '\n' );
    std::cout << output;
}


void Application::play_command( ) {
    PipedProcess process;
    process.command( "env | grep PATH" );
    std::string output = read_to_delim( process, '\n' );
    std::cout << output << std::endl;
}

void Application::play_bash( )
{
    PipedProcess process;
    std::string reply;
    const char* const argv[] = {nullptr};
    process.start(std::string("/bin/bash"), argv );

    process.write( "cd /proc\n" );
    process.write( "pwd\n" );
    reply = read_to_delim( process, '\n' );
    std::cout << reply << std::flush;

    process.write( "var=\"Hello bash auto\"\n" );
    process.write( "echo \"${var}\"\n" );
    reply = read_to_delim( process, '\n' );
    std::cout << reply << std::flush;

}

void Application::play_timeout( )
{
    cpp_context {
        PipedProcess process;
        process.set_timeout(5000 );
        process.command( std::string( "sleep 3; echo \"good morning\"\n" ));
        std::string reply = read_to_delim( process, '\n' );
        std::cout << reply << std::flush;
    }

    cpp_context {
        try {
            PipedProcess process;
            process.set_timeout( 5000 );
            process.command( std::string( "sleep 8; echo \"good morning\"\n" ));
            std::string reply = read_to_delim( process, '\n' );
            std::cout << "something went wrong. shouldn't be here" << std::endl;
        }
        catch( const timeout_exception& ex ) {
            std::cout << "time out as expected" << std::endl;
        }
    };
}