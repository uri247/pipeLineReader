#pragma once
#include <stdexcept>

class timeout_exception :public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};


class Pipe
{
private:
    int _value[2];

public:
    Pipe() : _value{-1, -1} { }

    virtual ~Pipe() {
        close( read_end() );
        close( write_end() );
    }

    void pipe();
    int& read_end() { return _value[0]; }
    int& write_end() { return _value[1]; }

public:
    void close(int& fd);
};


class PipedProcess
{
private:
    Pipe m_parent_to_child_pipe;
    Pipe m_child_to_parent_pipe;

public:
    void start(const std::string& cmd);
    int write(const std::string& msg);
    int read(char* buffer, int size);
};