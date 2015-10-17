
/*
* =====================================================================================
*
*       Filename:  pap_dijkstra.cpp
*
*    Description:
*
*        Version:  1.0
*        Created:  10/15/2015 10:32:36 PM
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Paul Thieme
*   Organization:
*
* =====================================================================================
*/

#include "pap_dijkstra.h"
#include "cppcl.hpp"

#include <iostream>
#include <vector>

#include <cstdlib>
#include <ctime>

using namespace std;


void generateGraph(GraphData* data, int vertexCount, int neighborsPerVertex)
{
    data->vertexCount = vertexCount;
    data->vertexArray = new int[vertexCount];

    data->edgeCount = vertexCount * neighborsPerVertex;
    data->edgeArray = new int[data->edgeCount];

    data->weightArray = new int[data->edgeCount];

    for (int i = 0; i < vertexCount; ++i)
        data->vertexArray[i] = i * neighborsPerVertex;

    for (int i = 0; i < data->edgeCount; ++i)
    {
        data->edgeArray[i] = rand() % vertexCount;
        data->weightArray[i] = rand() % 60;
    }
}


bool frontierEmpty(const cl_int* frontier, int vertexCount)
{
    bool running = true;
    for (int i = 0; running && i < vertexCount; ++i)
    {
        if (frontier[i])
            running = false;
    }
    return running;
}

int frontierSize(const cl_int* frontier, int vertexCount)
{
    int count = 0;
    for (int i = 0; i < vertexCount; ++i)
    {
        if (frontier[i] == 1)
            ++count;
    }
    return count;
}


/**
 * @fn  void runBreadthFirstSearch(cl::Context& context, GraphData& data, unsigned int startVertex, unsigned int endVertex)
 *
 * @brief   runs the breath first search on a given graph Uses the given OpenCL Context to run
 *          the algorithm.
 *          Uses a unweighted, one directional graph
 *          The weightArray of the Graph is therefor not used at all, and instead
 *          the weight is incremented by 1 in each step.
 *
 * @author  Paul Thieme
 * @date    10/17/2015
 *
 * @param [in]  context   The OpenCL Context.
 * @param [in]  data        The GraphData structure to run on.
 * @param   startVertex     The Vertex in the Graph to start from.
 * @param   endVertex       The end vertex of the route
 */
void runBreadthFirstSearch(cl::Context& context, cl::Device& device, GraphData& data,
                           int startVertex, int endVertex)
{
    cl::CommandQueue queue(context);

    //  load OpenCL code
    auto kernelsrc = loadFileContent("kernels/bfs.cl");
    //  load the program code into an object and compile it
    //  checks for errors, prints the build log if any
    cl::Program prog(context, kernelsrc);
    try
    {
        prog.build();
    }
    catch (cl::Error err)
    {
        std::cerr << prog.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
        throw;
    }

    cl_int bufStartVertex = startVertex;
    cl_int bufVertexCount = data.vertexCount;
    cl_int bufEdgeCount = data.edgeCount;
    //  setup the buffers for the GPU
    cl::Buffer bufVertex(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(cl_int) * bufVertexCount, data.vertexArray);
    cl::Buffer bufEdges(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(cl_int) * bufEdgeCount, data.edgeArray);

    //  will be generated and used only by the GPU
    //  -> no copying from host to device, only AFTER processing for the result
    cl::Buffer bufFrontier(context, CL_MEM_READ_WRITE, sizeof(cl_int) * bufVertexCount);
    cl::Buffer bufVisited (context, CL_MEM_READ_WRITE, sizeof(cl_int) * bufVertexCount);
    cl::Buffer bufCosts   (context, CL_MEM_READ_WRITE, sizeof(cl_int) * bufVertexCount);
    cl::Buffer bufPrevious(context, CL_MEM_READ_WRITE, sizeof(cl_int) * bufVertexCount);

    cl::Kernel kernelInit(prog, "bfs_init");
    cl::Kernel kernelStageOne(prog, "bfs_stage");

    try {
        kernelInit.setArg(0, bufFrontier);
        kernelInit.setArg(1, bufVisited);
        kernelInit.setArg(2, bufCosts);
        kernelInit.setArg(3, bufPrevious);
        kernelInit.setArg(4, bufVertexCount);
        kernelInit.setArg(5, bufStartVertex);

        kernelStageOne.setArg(0, bufVertex);
        kernelStageOne.setArg(1, bufEdges);
        kernelStageOne.setArg(2, bufFrontier);
        kernelStageOne.setArg(3, bufVisited);
        kernelStageOne.setArg(4, bufCosts);
        kernelStageOne.setArg(5, bufPrevious);
        kernelStageOne.setArg(6, bufVertexCount);
        kernelStageOne.setArg(7, bufEdgeCount);
    }
    catch (cl::Error err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << err.err() << std::endl;
        throw;
    }

    //  initialize buffers on the GPU
    queue.enqueueNDRangeKernel(kernelInit, cl::NullRange, cl::NDRange(bufVertexCount));

    {
        auto ptrVisited = (cl_int*)queue.enqueueMapBuffer(bufVisited, CL_TRUE, CL_MAP_READ, 0, sizeof(cl_int) * bufVertexCount);
        auto visitedSize = frontierSize(ptrVisited, bufVertexCount);
        std::cout << "Visited Size before: " << visitedSize << std::endl;
    }
    
    //  run BFS until finish
    //  calculate more than one stage each loop to keep the GPU busy
    unsigned int i = 0;
    bool keepRunning = true;
    do 
    {
        auto ptrFrontier = (cl_int*)queue.enqueueMapBuffer(bufFrontier, CL_TRUE, CL_MAP_READ, 0, sizeof(cl_int) * bufVertexCount);
        auto frontSize = frontierSize(ptrFrontier, bufVertexCount);
        std::cout << "Vorher: " << frontSize << ", ";
        queue.enqueueUnmapMemObject(bufFrontier, ptrFrontier);

        queue.enqueueNDRangeKernel(kernelStageOne, cl::NullRange, cl::NDRange(bufVertexCount));
        //queue.enqueueNDRangeKernel(kernelStageOne, cl::NullRange, cl::NDRange(bufVertexCount));
        //queue.enqueueNDRangeKernel(kernelStageOne, cl::NullRange, cl::NDRange(bufVertexCount));
        //queue.enqueueNDRangeKernel(kernelStageOne, cl::NullRange, cl::NDRange(bufVertexCount));
        //queue.enqueueNDRangeKernel(kernelStageOne, cl::NullRange, cl::NDRange(bufVertexCount));

        //  Map Frontier Buffer into host memory and check if it is empty
        //  If empty exit loop and print results, otherwise keep running
        ptrFrontier = (cl_int*)queue.enqueueMapBuffer(bufFrontier, CL_TRUE, CL_MAP_READ, 0, sizeof(cl_int) * bufVertexCount);
        frontSize = frontierSize(ptrFrontier, bufVertexCount);
        //  cleanup memory so we can use it in the kernel again
        queue.enqueueUnmapMemObject(bufFrontier, ptrFrontier);

        std::cout << "nachher " << frontSize << std::endl;
        ++i;
        keepRunning = frontSize;
    } while (keepRunning);

    std::cout << "Loops needed: " << i << std::endl;
    return;
}

/**
 * @fn  void runDijkstra(int argc, char* argv[])
 *
 * @brief   Executes the dijkstra operation.
 *
 * @author  Paul Thieme
 * @date    10/17/2015
 *
 * @param   argc            The argc.
 * @param [in,out]  argv    If non-null, the argv.
 */
void runDijkstra(int argc, char* argv[])
{
    srand(time(nullptr));

    int vertexCount  = 500000;
    int edgesPerVert = 5;
    int startVertex = rand() % vertexCount;
    int endVertex = rand() % vertexCount;

    cout << "Vertex count: " << vertexCount << ", (";
    cout << (sizeof(int) * vertexCount) / 1024 / 1024 << " MB)" << std::endl;
    cout << "Edge   count: " << vertexCount*edgesPerVert << ", (";
    cout << (sizeof(int) * vertexCount * edgesPerVert) / 1024 / 1024 << " MB)" << std::endl;

    GraphData data;
    generateGraph(&data, vertexCount, edgesPerVert);

    try {
        //  get all available platforms
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        //  select a platform
        auto selectedPlatform = selectClPlatform(platforms);
        std::cout << "You choose Platform #" << platforms[selectedPlatform].getInfo<CL_PLATFORM_NAME>() << std::endl;

        //  select a specific device on that platform
        auto selectedDevice = selectClDevice(platforms[selectedPlatform]);
        std::cout << "You choose Device #" << selectedDevice.getInfo<CL_DEVICE_NAME>() << std::endl;

        //  create a OpenCL context for that device
        cl::Context context(selectedDevice);
        runBreadthFirstSearch(context, selectedDevice, data, startVertex, endVertex);
    }
    catch (cl::Error& err)
    {
        std::cerr << "Whoupdee*fucking*doo" << std::endl;
        std::cerr << err.what() << std::endl;
        std::cerr << err.err() << std::endl;
    }
    cin.get();
}
