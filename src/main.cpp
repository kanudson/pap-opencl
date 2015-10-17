
#include <iostream>
#include <vector>
#include <string>

#include "cppcl.hpp"
#include "pap_dijkstra.h"


//  entry points for testing
int convolve(int argc, char*argv[]);
int ocl_cpphello(int argc, char** argv);


int main(int argc, char* argv[])
{
    try {
        runDijkstra(argc, argv);
    }
    catch (const char* msg)
    {
        std::cerr << msg << std::endl;
    }

    return 0;
}
