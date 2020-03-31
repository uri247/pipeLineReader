#include <iostream>
#include <string>
#include <system_error>
#include <cstring>
#include <unistd.h>
#include "parent.h"
#include "pipe.h"


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


int Application::main( ) {
    PipedProcess process;

    process.start( "/bin/bash" );

    std::string msg{"df\n"};
    int wrote = process.write( msg );
    std::cout << "wrote " << wrote << " characters" << std::endl;

    char buff[500];
    while(true) {
        int red = process.read(buff, 300);
        if( red == 0 )
            break;
        buff[red] = 0x00;
        std::cout << buff << std::flush;
    }

    return 0;
}