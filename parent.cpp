#include <iostream>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <array>
#include "parent.h"
#include "pipe.h"
#pragma ide diagnostic ignored "MemberFunctionCanBeStatic"


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
//    play_bash();
    play_child();
    return 0;
}

std::string Application::get_msg(PipedProcess& process)
{
    std::string msg;
    bool keep = true;

    do {
        std::array<char, 1020> buff{};
        int i;

        int red = process.read( &buff[ 0 ], 1000 );
        if( red == 0 ) {
            keep = false;
        }
        else {
            for( i = 0; i < red; ++i ) {
                if( buff[ i ] == 0 ) {
                    keep = false;
                    break;
                }
            }
            msg += std::string( &buff[ 0 ], i );
        }
    }
    while( keep );

    return msg;
}

void Application::play_child( )
{
    PipedProcess process;
    char buff[1020];
    std::string msg;

    process.start( "./child" );
    msg = get_msg( process );
    std::cout << msg << std::endl;

    for( int i=0; i<10000000; ++i )
    {
        process.write( std::string("RECORD\n"));
        msg = get_msg( process );
        std::cout << msg << std::endl;
        if( msg.length() == 0 )
            break;
    }
    process.write( std::string( "QUIT\n"));
}

void Application::play_bash( )
{
    PipedProcess process;

    process.start( "/bin/bash" );
    process.write( std::string("lsb_release -i -c -r -d\n"));
    process.write( std::string("exit\n"));

    char buff[1020];
    while(true) {
        int red = process.read(buff, 1000);
        if( red == 0 )
            break;
        buff[red] = 0x00;
        std::cout << buff << std::flush;
    }

}
