
#include "cppcl.hpp"


namespace
{
    //  convolve data
    constexpr unsigned int INPUT_WIDTH = 8;
    constexpr unsigned int INPUT_HEIGHT = 8;
    constexpr unsigned int OUTPUT_WIDTH = 6;
    constexpr unsigned int OUTPUT_HEIGHT = 6;
    constexpr unsigned int MASK_WIDTH = 3;
    constexpr unsigned int MASK_HEIGHT = 3;

    cl_uint mask[MASK_WIDTH][MASK_HEIGHT] =
    {
        { 1,1,1 },{ 1,0,1 },{ 1,1,1 }
    };

    cl_uint outputData[OUTPUT_WIDTH][OUTPUT_HEIGHT];

    cl_uint inputSignal[INPUT_WIDTH][INPUT_WIDTH] =
    {
        { 3, 1, 1, 4, 8, 2, 1, 3 },
        { 4, 2, 1, 1, 2, 1, 2, 3 },
        { 4, 4, 4, 4, 3, 2, 2, 2 },
        { 9, 8, 3, 8, 9, 0, 0, 0 },
        { 9, 3, 3, 9, 0, 0, 0, 0 },
        { 0, 9, 0, 8, 0, 0, 0, 0 },
        { 3, 0, 8, 8, 9, 4, 4, 4 },
        { 5, 9, 8, 1, 8, 1, 1, 1 }
    };


    int selectClPlatform(std::vector<cl::Platform> platforms)
    {
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

        //  skip selection if only one platform is available
        if (i == 1)
            return 0;

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

        //  skip selection if only one device is available
        if (i == 1)
            return devices[0];

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

}   //  namespace


int ocl_cpphello(int argc, char** argv)
{
    try {
        //  get all available platforms
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        //  select a platform
        auto selectedPlatform = selectClPlatform(platforms);
        std::cout << "You choose Platform #" << selectedPlatform << std::endl;

        //  select a specific device on that platform
        auto selectedDevice = selectClDevice(platforms[selectedPlatform]);
        std::cout << "You choose Device #" << selectedDevice.getInfo<CL_DEVICE_NAME>() << std::endl;

        //  create a OpenCL context for that device
        cl::Context context(selectedDevice);
        //  create command queue for the context
        cl::CommandQueue que(context);

        auto kernelsrc = loadFileContent("kernels/convolution.cl");
        cl::Program prog(context, kernelsrc, true);

        //  setup the buffers for in-/output
        cl::Buffer bufferInput(context,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            INPUT_WIDTH * INPUT_HEIGHT * sizeof(cl_uint),
            inputSignal);
        cl::Buffer bufferMask(context,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            MASK_WIDTH * MASK_HEIGHT * sizeof(cl_uint),
            mask);
        cl::Buffer bufferOutput(context,
            CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
            OUTPUT_WIDTH * OUTPUT_HEIGHT * sizeof(cl_uint),
            outputData);

        cl::Kernel kernel(prog, "convolve");
        kernel.setArg(0, bufferInput);
        kernel.setArg(1, bufferMask);
        kernel.setArg(2, bufferOutput);
        kernel.setArg(3, sizeof(cl_uint), &INPUT_WIDTH);
        kernel.setArg(4, sizeof(cl_uint), &MASK_WIDTH);
        
        //  run the kernel
        que.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(INPUT_WIDTH * INPUT_HEIGHT));
        //  read the output and write it to the output buffer
        que.enqueueReadBuffer(bufferOutput, true, 0, sizeof(cl_uint) * OUTPUT_WIDTH * OUTPUT_HEIGHT, outputData);

        //  print the result
        for (int y = 0; y < OUTPUT_WIDTH; y++)
        {
            for (int x = 0; x < OUTPUT_WIDTH; x++)
            {
                std::cout << outputData[x][y] << " ";
            }
            std::cout << std::endl;
        }
    }
    catch (...)
    {
        std::cerr << "Whoupdee*fucking*doo" << std::endl;
    }
    return 0;
}
