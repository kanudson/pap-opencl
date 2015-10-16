
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "cppcl.hpp"

cl_uint* createInputData(unsigned int seed = 0);

namespace {
    constexpr unsigned int INPUT_WIDTH   = 8;
    constexpr unsigned int INPUT_HEIGHT  = 8;
    constexpr unsigned int OUTPUT_WIDTH  = 6;
    constexpr unsigned int OUTPUT_HEIGHT = 6;
    constexpr unsigned int MASK_WIDTH    = 3;
    constexpr unsigned int MASK_HEIGHT   = 3;

    cl_uint mask[MASK_WIDTH][MASK_HEIGHT] = 
        {
            {1,1,1}, {1,0,1}, {1,1,1}
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

}

//// Callback stuff
//
void CL_CALLBACK contextCallback(
        const char* errInfo,
        const void* privateInfo,
        size_t cb,
        void* userdata)
{
    std::cerr << "Error occurred during context in use: " << errInfo << std::endl;
    exit(EXIT_FAILURE);
}


int convolve(int argc, char* argv[])
{
    cl_int err = CL_SUCCESS;
    auto platformList = papcl::getPlatformList();

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
        std::cerr << "nope, kein GPU context fuer dich" << std::endl;
        cl_context context = clCreateContextFromType(props,
               CL_DEVICE_TYPE_CPU,
               nullptr, nullptr, &err);
        if (err != CL_SUCCESS)
        {
            std::cerr << "nope, auch kein CPU context da =/" << std::endl;
            return 1;
        }
    }
    else
    {
        std::cout << "GPU Context auf Platform [0] erstellt" << std::endl;
    }

    size_t paramsize;
    err = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, nullptr, &paramsize);
    std::vector<cl_device_id> deviceids(paramsize / sizeof(cl_device_id));
    err = clGetContextInfo(context, CL_CONTEXT_DEVICES, paramsize, deviceids.data(), nullptr);

    for (auto x : deviceids)
        std::cout << x << std::endl;

    std::ifstream srcfile("kernels/convolution.cl");
    std::string   srcprog(std::istreambuf_iterator<char>(srcfile),
            (std::istreambuf_iterator<char>()));
    const char* src = srcprog.c_str();
    size_t srclength = srcprog.length();

    auto program = clCreateProgramWithSource(context,
            1, &src, &srclength, &err);
    if (err != CL_SUCCESS)
        throw "nope, kein program fuer dich";

    err = clBuildProgram(program, 1, &deviceids[0], nullptr, nullptr, nullptr);
    if (err == -11)
    {
        size_t size;
        clGetProgramBuildInfo(program, deviceids[0], CL_PROGRAM_BUILD_LOG, 0, nullptr, &size);
        std::vector<char> log(size + 1);
        clGetProgramBuildInfo(program, deviceids[0], CL_PROGRAM_BUILD_LOG, size, log.data(), nullptr);
        log[size + 1] = 0;
        std::cerr << log.data() << std::endl;
        return 1;
    }
    else if (err != CL_SUCCESS)
        throw "nope, kein compail fuer dich";

    auto kernel = clCreateKernel(program, "convolve", &err);
    if (err != CL_SUCCESS)
        throw "kein kernel fuer dich";

    std::cout << "doing stuff... " << std::endl;
    //  create data
    auto data = inputSignal;
    std::cout << "input data initialized" << std::endl;

    //  Puffer
    auto inputBuffer = clCreateBuffer(context,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof(cl_uint) * INPUT_WIDTH * INPUT_HEIGHT,
            static_cast<void*>(data), &err);
    if (err != CL_SUCCESS)
    {
        std::cerr << "inputpuffer kann nicht erstellt werden" << std::endl;
        return 1;
    }

    auto maskBuffer = clCreateBuffer(context,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof(cl_uint) * MASK_WIDTH * MASK_HEIGHT,
            static_cast<void*>(mask), &err);
    if (err != CL_SUCCESS)
    {
        std::cerr << "inputpuffer kann nicht erstellt werden" << std::endl;
        return 1;
    }

   auto outputBuffer = clCreateBuffer(context,
            CL_MEM_WRITE_ONLY,
            sizeof(cl_uint) * OUTPUT_WIDTH * OUTPUT_HEIGHT,
            nullptr, &err);
    if (err != CL_SUCCESS)
        papcl::printClError(err, "output buffer kann nicht erstellt werden");

    auto queue = clCreateCommandQueue(context,
        deviceids[0], 0, &err);
    if (err != CL_SUCCESS)
        papcl::printClError(err, "keine queue");

    err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputBuffer);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &maskBuffer);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &outputBuffer);
    err |= clSetKernelArg(kernel, 3, sizeof(cl_uint), &INPUT_WIDTH);
    err |= clSetKernelArg(kernel, 4, sizeof(cl_uint), &MASK_WIDTH);
    if(err != CL_SUCCESS)
        papcl::printClError(err, "argument shti failed");

    const size_t globalWorkSize[1] = { OUTPUT_WIDTH * OUTPUT_HEIGHT };
    const size_t localWorkSize[1]  = { 1 };

    err = clEnqueueNDRangeKernel(queue, kernel, 1, nullptr,
            globalWorkSize, localWorkSize,
            0, nullptr, nullptr);
    if (err != CL_SUCCESS)
        papcl::printClError(err, "enqueue will nich");

    //  ergebnis laden
    err = clEnqueueReadBuffer(queue, outputBuffer, CL_TRUE, 0,
            sizeof(cl_uint) * OUTPUT_WIDTH * OUTPUT_HEIGHT,
            outputData, 0, nullptr, nullptr);
    if (err != CL_SUCCESS)
        papcl::printClError(err, "read output failed");


    // Output the result buffer
    for (int y = 0; y < OUTPUT_WIDTH; y++)
    {
        for (int x = 0; x < OUTPUT_WIDTH; x++)
        {
            std::cout << outputData[x][y] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl << "Executed program succesfully." << std::endl;
    return 0;
}


/////////////////////////////////////////////////////////////////////
//  erstellt input array
//  pointer muss vom _aufrufer_ freigegeben werden
cl_uint* createInputData(unsigned int seed)
{
    srand(seed);

    auto data = new cl_uint[INPUT_WIDTH * INPUT_HEIGHT];
    for (cl_int row = 0; row < INPUT_HEIGHT; ++row)
        for (cl_int col = 0; col < INPUT_WIDTH; ++col)
            data[(row * INPUT_WIDTH) + col] = rand() % 6;
    return data;
}

