#pragma once
#include "pipe.h"


class Application
{
public:
    Application();
    int main();
    void play_bash();
    void play_child();

private:
    std::string get_msg(PipedProcess& process);
};

