#pragma once
#include <stdexcept>

class timeout_exception :public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};


class Pipe
{
public:
    enum end_type { read_end=0, write_end=1 };

private:
    int _value[2];

public:
    Pipe() : _value{-1, -1} { }

    virtual ~Pipe() {
        close( read_end );
        close( write_end );
    }

    void pipe();
    int& read_fd() { return _value[read_end]; }
    int& write_fd() { return _value[write_end]; }

public:
    void close(end_type end);
};


class PipedProcess
{
private:
    Pipe m_parent_to_child_pipe;
    Pipe m_child_to_parent_pipe;

public:
    void command(const std::string& cmd);
    void start(const std::string& path, const char *const argv[]);
    int write(const std::string& msg);
    int read(char* buffer, int size);
};