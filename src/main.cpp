
#include <iostream>

#include <CL/cl.h>


int main(int argc, char* argv[])
{
    cl_uint numPlatforms = 0;
    clGetPlatformIDs(0, nullptr, &numPlatforms);

    std::cout << "Number of Platforms: " << numPlatforms << std::endl;

    return 0;
}

