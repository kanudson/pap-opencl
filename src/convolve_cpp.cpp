
#include "cppcl.hpp"



namespace
{
    int selectClPlatform(std::vector<cl::Platform> platforms)
    {
        unsigned int i = 0;
        for (auto x : platforms)
        {
            std::cout << "PlatformID #" << i++ << std::endl;
            auto info = platforms[0].getInfo<CL_PLATFORM_VENDOR>();
            std::cout << "\t" << info << std::endl;

            auto info2 = platforms[0].getInfo<CL_PLATFORM_NAME>();
            std::cout << "\t" << info2 << std::endl;

            auto info3 = platforms[0].getInfo<CL_PLATFORM_VERSION>();
            std::cout << "\t" << info3 << std::endl;

            auto info4 = platforms[0].getInfo<CL_PLATFORM_PROFILE>();
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

    int selectClDevice(cl::Platform& platform)
    {
        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_CPU | CL_DEVICE_TYPE_GPU, &devices);

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
            std::cout << "\t\tDevice Max Allocateable Memory: " << device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>() << std::endl;
            std::cout << "\t\tDevice Local Memory: " << device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() << std::endl;
            std::cout << "\t\tDevice Available: " << device.getInfo< CL_DEVICE_AVAILABLE>() << std::endl;
        }

        unsigned int selection;
        do
        {
            std::cout << "Select DeviceID [0-" << i - 1 << "]: ";
            std::cin >> selection;
        } while (selection >= i);
        return selection;
    }

}   //  namespace


int ocl_cpphello(int argc, char** argv)
{
    try {
        //  get all available platforms
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        auto selectedPlatform = selectClPlatform(platforms);
        std::cout << "You choose #" << selectedPlatform << std::endl;

        auto selectedDevice = selectClDevice(platforms[selectedPlatform]);
        std::cout << "You choose #" << selectedDevice << std::endl;
    }
    catch (...)
    {
        std::cerr << "Whoupdee*fucking*doo" << std::endl;
    }
    return 0;
}
