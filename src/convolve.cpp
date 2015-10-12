
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "cppcl.hpp"

void parsePlatformInfo(cl_platform_id platform);
cl_int* createInputData(unsigned int seed = 0);

namespace {
    constexpr unsigned int INPUT_WIDTH   = 10000;
    constexpr unsigned int INPUT_HEIGHT  = 10000;
    constexpr unsigned int OUTPUT_WIDTH  = INPUT_WIDTH - 2;
    constexpr unsigned int OUTPUT_HEIGHT = INPUT_HEIGHT - 2;
    constexpr unsigned int MASK_WIDTH    = 3;
    constexpr unsigned int MASK_HEIGHT   = 3;
    
    const cl_int mask[MASK_WIDTH][MASK_HEIGHT] = 
        {
            {1,1,1}, {1,0,1}, {1,1,1}
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

    //  create data
    auto data = createInputData(1);

    delete data;
    return 0;
}


/////////////////////////////////////////////////////////////////////
//  erstellt input array
//  pointer muss vom _aufrufer_ freigegeben werden
cl_int* createInputData(unsigned int seed)
{
    srand(seed);

    cl_int* data = new cl_int[INPUT_WIDTH * INPUT_HEIGHT];
    for (cl_int row = 0; row < INPUT_HEIGHT; ++row)
        for (cl_int col = 0; col < INPUT_WIDTH; ++col)
            data[(row * INPUT_WIDTH) + col] = rand() % 6;
    return data;
}

