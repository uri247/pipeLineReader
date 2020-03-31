#include <iostream>
#include <string>
#include <unistd.h>
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
    play_bash();
    return 0;
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
