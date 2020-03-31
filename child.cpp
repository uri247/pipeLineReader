#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>


std::string get_quote(std::istream& strm)
{
    std::ostringstream ss{};
    std::string empty;
    std::string space;
    std::vector<std::string> lines;


    while( true ) {
        // read all lines into vector. For each line, remove the trailing '\r'
        std::string line;
        bool result = (bool) std::getline( strm, line );
        if( !result || line.length() == 1 )
            break;
        line.pop_back();
        lines.push_back( std::move( line ));
    }

    int i=0;
    ss << lines[i++];
    while( i+1 < lines.size() ) {
        ss << ' ' << lines[ i++ ];
    }
    ss << std::endl;
    ss << lines[i];

    return ss.str();
}

int main()
{
    std::ifstream q_file("quotes.txt");
    std::string line;

    // tell parent that chile is ready
    std::cout << "READY\r\n" << std::flush;

    while( static_cast<bool>(q_file) ) {
        std::string request;
        std::cin >> request;

        if( request == "QUIT" ) {
            break;
        }
        if( request == "RECORD" ) {
            std::string quote = get_quote(q_file);
            std::cout << quote << std::endl << '\0' << std::flush;
        }
    }

    return 0;
}