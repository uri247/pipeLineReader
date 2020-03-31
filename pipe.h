#pragma once

class Pipe
{
private:
    int _value[2];

public:
    enum end_type {read_end=0, write_end=1};

public:
    Pipe() : _value{-1, -1} { }

    virtual ~Pipe() {
        close_both();
    }

    void pipe();
    int& read_pipe() { return _value[read_end]; }
    int& write_pipe() { return _value[write_end]; }
    void close(end_type end);

    void close_both() {
        close( read_end );
        close( write_end );
    }

};


class PipedProcess
{
private:
    Pipe m_stdin_pipe;
    Pipe m_stdout_pipe;

public:
    void start(const std::string& cmd);
    int write(std::string msg);
    int read(char* buffer, int size);
};