
#include "cppcl.hpp"
#include "pap_dijkstra.h"


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

        cl_uint inpWidth = INPUT_WIDTH;
        cl_uint masWidth = MASK_WIDTH;

        cl::Kernel kernel(prog, "convolve");
        kernel.setArg(0, bufferInput);
        kernel.setArg(1, bufferMask);
        kernel.setArg(2, bufferOutput);
        kernel.setArg(3, sizeof(cl_uint), &inpWidth);
        kernel.setArg(4, sizeof(cl_uint), &masWidth);
        
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
