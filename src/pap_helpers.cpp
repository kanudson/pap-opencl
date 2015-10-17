
#include "pap_dijkstra.h"



int selectClPlatform(std::vector<cl::Platform> platforms)
{
    return 0;
    if (platforms.size() == 1)
        return 0;

    unsigned int i = 0;
    for (auto platform : platforms)
    {
        std::cout << "PlatformID #" << i++ << std::endl;
        auto info = platform.getInfo<CL_PLATFORM_VENDOR>();
        std::cout << "\t" << info << std::endl;

        auto info2 = platform.getInfo<CL_PLATFORM_NAME>();
        std::cout << "\t" << info2 << std::endl;

        auto info3 = platform.getInfo<CL_PLATFORM_VERSION>();
        std::cout << "\t" << info3 << std::endl;

        auto info4 = platform.getInfo<CL_PLATFORM_PROFILE>();
        std::cout << "\t" << info4 << std::endl;
    }

    unsigned int selection;
    do
    {
        std::cout << "Select platform ID [0-" << i - 1 << "]: ";
        std::cin >> selection;
    } while (selection >= i);
    return selection;
}

cl::Device selectClDevice(cl::Platform& platform)
{
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_CPU | CL_DEVICE_TYPE_GPU, &devices);

    if (devices.size() == 1)
        return devices[0];

    unsigned int i = 0;
    for (auto device : devices)
    {
        std::cout << "\tDevice " << i++ << ": " << std::endl;
        std::cout << "\t\tDevice Name: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        std::cout << "\t\tDevice Type: " << device.getInfo<CL_DEVICE_TYPE>();
        std::cout << " (GPU: " << CL_DEVICE_TYPE_GPU << ", CPU: " << CL_DEVICE_TYPE_CPU << ")" << std::endl;
        std::cout << "\t\tDevice Vendor: " << device.getInfo<CL_DEVICE_VENDOR>() << std::endl;
        std::cout << "\t\tDevice Max Compute Units: " << device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl;
        std::cout << "\t\tDevice Global Memory: " << device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() << std::endl;
        std::cout << "\t\tDevice Max Clock Frequency: " << device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>() << std::endl;
        std::cout << "\t\tDevice Max Allocatable Memory: " << device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>() << std::endl;
        std::cout << "\t\tDevice Local Memory: " << device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() << std::endl;
        std::cout << "\t\tDevice Available: " << device.getInfo< CL_DEVICE_AVAILABLE>() << std::endl;
    }

    unsigned int selection;
    do
    {
        std::cout << "Select DeviceID [0-" << i - 1 << "]: ";
        std::cin >> selection;
    } while (selection >= i);
    return devices[selection];
}

std::string loadFileContent(std::string filename)
{
    std::ifstream sourcefile(filename, std::ios::in);

    if (!sourcefile)
    {
        std::string msg("Could not load file: ");
        msg.append(filename);
        throw msg;
    }

    std::stringstream buffer;
    buffer << sourcefile.rdbuf();
    return buffer.str();

}
