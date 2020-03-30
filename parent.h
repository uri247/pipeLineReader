#include <unistd.h>

class Application
{
public:
    int main();
};



class FdPair
{
private:
    int _value[2];

public:
    enum end_type {read_end=0, write_end=1};

public:
    FdPair() : _value{-1, -1} { }

    virtual ~FdPair() {
        close_both();
    }

    void pipe();
    int& read_pipe() { return _value[read_end]; }
    int& write_pipe() { return _value[write_end]; }

    void close(end_type end) {
        ::close(_value[end]);
        _value[end] = -1;
    }

    void close_both() {
        close( read_end );
        close( write_end );
    }

};