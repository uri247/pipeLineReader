#include <iostream>
#include <unistd.h>



int main()
{
    //execl("/bin/bash", "prog", "-c", "ls", nullptr);
    execl("/bin/bash", "prog", nullptr);
    std::cout << "Shouldn't be here" << std::endl <<
        "Error code: " << *__errno_location() << std::endl;
}