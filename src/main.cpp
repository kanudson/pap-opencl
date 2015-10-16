
#include <iostream>
#include <vector>
#include <string>

#include "cppcl.hpp"


//  entry points
int convolve(int argc, char*argv[]);
int ocl_cpphello(int argc, char** argv);


int main(int argc, char* argv[])
{
    try {
        ocl_cpphello(argc, argv);
    }
    catch (const char* msg)
    {
        std::cerr << msg << std::endl;
    }

    return 0;
}
