
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
#include <cstdlib>
#include <vector>

using namespace std;


void generateGraph(GraphData* data, int vertexCount, int neighborsPerVertex)
{
    data->vertexCount = vertexCount;
    data->vertexArray = new unsigned int[vertexCount];

    data->edgeCount = vertexCount * neighborsPerVertex;
    data->edgeArray = new unsigned int[data->edgeCount];

    data->weightArray = new unsigned int[data->edgeCount];

    for (int i = 0; i < vertexCount; ++i)
        data->vertexArray[i] = i * neighborsPerVertex;

    for (int i = 0; i < data->edgeCount; ++i)
    {
        data->edgeArray[i] = rand() % vertexCount;
        data->weightArray[i] = rand() % 60;
    }
}


bool frontierEmpty(const cl_int* frontier, unsigned int vertexCount)
{
    bool running = true;
    for (unsigned int i = 0; running && i < vertexCount; ++i)
    {
        if (frontier[i])
            running = false;
    }
    return running;
}


/**
 * @fn  void runBreadthFirstSearch(cl::Context& context, GraphData& data, unsigned int startVertex, unsigned int endVertex)
 *
 * @brief   runs the breath first search on a given graph Uses the given OpenCL Context to run
 *          the algorithm.
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
                           unsigned int startVertex, unsigned int endVertex)
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

    cl_uint bufStartVertex = startVertex;
    cl_int bufVertexCount = data.vertexCount;
    cl_int bufEdgeCount = data.edgeCount;
    //  setup the buffers for the GPU
    cl::Buffer bufVertex(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(cl_int) * bufVertexCount, data.vertexArray);
    cl::Buffer bufEdges(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(cl_int) * bufEdgeCount, data.edgeArray);

    //  will be generated and used only by the GPU
    //  -> no copying from host to device, only AFTER processing for the result
    cl::Buffer bufFrontier  (context, CL_MEM_READ_WRITE, sizeof(cl_int) * bufVertexCount);
    cl::Buffer bufVisited   (context, CL_MEM_READ_WRITE, sizeof(cl_int) * bufVertexCount);
    cl::Buffer bufCost      (context, CL_MEM_READ_WRITE, sizeof(cl_int) * bufVertexCount);
    cl::Buffer bufCostUpdate(context, CL_MEM_READ_WRITE, sizeof(cl_int) * bufVertexCount);
    cl::Buffer bufPrevious  (context, CL_MEM_READ_WRITE, sizeof(cl_int) * bufVertexCount);

    cl::Kernel kernelInit(prog, "bfs_init");
    kernelInit.setArg(0, bufFrontier);
    kernelInit.setArg(1, bufVisited);
    kernelInit.setArg(2, bufCost);
    kernelInit.setArg(3, bufCostUpdate);
    kernelInit.setArg(4, bufPrevious);
    kernelInit.setArg(5, bufVertexCount);
    kernelInit.setArg(6, bufStartVertex);

    //  initialize buffers on the GPU
    queue.enqueueNDRangeKernel(kernelInit, cl::NullRange, cl::NDRange(bufVertexCount));
    //  read the output and write it to the output buffer
    auto ptrFrontier = (cl_int*)queue.enqueueMapBuffer(bufFrontier,   CL_TRUE, CL_MAP_READ, 0, sizeof(cl_int) * bufVertexCount);
    auto ptrVisited  = (cl_int*)queue.enqueueMapBuffer(bufVisited,    CL_TRUE, CL_MAP_READ, 0, sizeof(cl_int) * bufVertexCount);
    auto ptrCost     = (cl_int*)queue.enqueueMapBuffer(bufCost,       CL_TRUE, CL_MAP_READ, 0, sizeof(cl_int) * bufVertexCount);
    auto ptrCostUp   = (cl_int*)queue.enqueueMapBuffer(bufCostUpdate, CL_TRUE, CL_MAP_READ, 0, sizeof(cl_int) * bufVertexCount);
    auto ptrPrev     = (cl_int*)queue.enqueueMapBuffer(bufPrevious,   CL_TRUE, CL_MAP_READ, 0, sizeof(cl_int) * bufVertexCount);

    std::cout << "Frontier is empty? " << frontierEmpty(ptrFrontier, bufVertexCount) << std::endl;

    //  print the result
    for (unsigned int i = 0; i < bufVertexCount; ++i)
    {
        std::cout << i << "\t";
        std::cout << *(ptrFrontier + i) << "\t";
        std::cout << *(ptrVisited  + i) << "\t";
        std::cout << *(ptrCost + i)     << "\t";
        std::cout << *(ptrCostUp + i)   << "\t";
        std::cout << *(ptrPrev + i)     << "\t" << std::endl;
    }

    cl::Kernel kernelStageOne(prog, "bfs_stage_one");
    kernelStageOne.setArg(0, bufVertex);
    kernelStageOne.setArg(1, bufEdges);
    kernelStageOne.setArg(2, bufFrontier);
    kernelStageOne.setArg(3, bufVisited);
    kernelStageOne.setArg(4, bufCost);
    kernelStageOne.setArg(5, bufCostUpdate);
    kernelStageOne.setArg(6, bufPrevious);
    kernelStageOne.setArg(7, bufVertexCount);
    kernelStageOne.setArg(8, bufEdgeCount);

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
    GraphData data;
    generateGraph(&data, 300, 5);

    cout << "Vertex count: " << data.vertexCount << std::endl;
    cout << "Edge   count: " << data.edgeCount   << std::endl;

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
        runBreadthFirstSearch(context, selectedDevice, data, 0, 200);
    }
    catch (cl::Error& err)
    {
        std::cerr << "Whoupdee*fucking*doo" << std::endl;
        std::cerr << err.what() << std::endl;
        std::cerr << err.err() << std::endl;
    }
    cin.get();
}
