
#include <iostream>
#include <vector>
#include <string>

#include "cppcl.hpp"


void parsePlatformInfo(cl_platform_id platform);
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

    cl_int err = CL_SUCCESS;
    auto platformList = ocl::getPlatformList();

    cl_context_properties props[] = {
        CL_CONTEXT_PLATFORM,
        (cl_context_properties)platformList[0],
        0
    };
    cl_context context = clCreateContextFromType(props,
            CL_DEVICE_TYPE_GPU,
            nullptr, nullptr, &err);
    if (err != CL_SUCCESS)
    {
        std::cerr << "nope, kein context fuer dich" << std::endl;
        return 1;
    }
    else
    {
        std::cout << "GPU Context auf Platform [0] erstellt" << std::endl;
    }
    
    //  Get information for each platform
    for (auto id : platformList)
    {
        parsePlatformInfo(id);
    }

    return 0;
}

void parsePlatformInfo(cl_platform_id platform)
{
    cl_int err = 0;
    std::size_t length;

    auto platformName    = ocl::get_clPlatformInfo(platform, CL_PLATFORM_NAME);
    auto platformVendor  = ocl::get_clPlatformInfo(platform, CL_PLATFORM_VENDOR);
    auto platformProfile = ocl::get_clPlatformInfo(platform, CL_PLATFORM_PROFILE);
    auto platformVersion = ocl::get_clPlatformInfo(platform, CL_PLATFORM_VERSION);
    
    std::cout << platform << ", " << platformName << std::endl;
    std::cout << platform << ", " << platformVendor << std::endl;
    std::cout << platform << ", " << platformProfile << std::endl;
    std::cout << platform << ", " << platformVersion << std::endl;
}

