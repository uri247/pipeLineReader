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

//    std::string msg{"ls\n"};
//    int wrote = process.write( msg );
//    std::cout << "wrote " << wrote << " characters" << std::endl;

    char buff[500];
    while( process.read( buff, 500 )) {
        std::cout << buff;
    }

    return 0;
}