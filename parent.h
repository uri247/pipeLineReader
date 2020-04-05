#pragma once
#include <string>
#include "pipe.h"


class Application
{
public:
    Application();
    int main();

private:
    void play_bash();
    void play_command();
    void play_child();
    void play_timeout();
    void play_start();

    std::string get_msg(PipedProcess& process);
    std::string read_to_delim( PipedProcess& process, char delim);
};

