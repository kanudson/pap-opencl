
#include <iostream>
#include <vector>
#include <string>

#include "cppcl.hpp"


int convolve(int argc, char*argv[]);

int main(int argc, char* argv[])
{
    try {
        convolve(argc, argv);
    }
    catch (const char* msg)
    {
        std::cerr << msg << std::endl;
    }
    return 0;
}
