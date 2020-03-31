#include <iostream>
#include <unistd.h>



int main()
{
    const char* argv[] = {"arg0", "-c", "df", nullptr };
    execv("/bin/bash", const_cast<char**>(argv));

    std::cout << "Shouldn't be here" << std::endl <<
        "Error code: " << *__errno_location() << std::endl;
}