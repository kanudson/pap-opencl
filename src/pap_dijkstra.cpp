
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
#include <ctime>

using namespace std;


bool frontierEmpty(const cl_uint* frontier, uint64_t vertexCount)
{
    bool running = true;
    for (int i = 0; running && i < vertexCount; ++i)
    {
        if (frontier[i])
            running = false;
    }
    return running;
}

int frontierSize(const cl_uint* frontier, uint64_t vertexCount)
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
 *          Uses an unweighted, one directional graph
 *          The weightArray of the Graph is therefor not used at all, and instead
 *          the weight is incremented by 1 in each step.
 *
 * @author  Paul Thieme
 * @date    10/17/2015
 *
 * @param [in]  context     The OpenCL Context.
 * @param [in]  data        The GraphData structure to run on.
 * @param   startVertex     The Vertex in the Graph to start from.
 * @param   endVertex       The end vertex of the route
 */
void runBreadthFirstSearch(cl::Context& context, cl::Device& device, GraphData& graph,
                           uint32_t startVertex, uint32_t endVertex, std::ostream& ss)
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
        ss << prog.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
        throw;
    }

    cl_uint bufStartVertex = startVertex;
    cl_ulong bufVertexCount = graph.VertexCount();
    cl_ulong bufEdgeCount = graph.EdgeCount();
    //  setup the buffers for the GPU
    cl::Buffer bufVertex(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(cl_uint) * bufVertexCount, graph.GetVertices());
    cl::Buffer bufEdges(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(cl_uint) * bufEdgeCount, graph.GetEdges());

    //  will be generated and used only by the GPU
    //  -> no copying from host to device, only AFTER processing for the result
    cl::Buffer bufFrontier(context, CL_MEM_READ_WRITE, sizeof(cl_uint) * bufVertexCount);
    cl::Buffer bufVisited (context, CL_MEM_READ_WRITE, sizeof(cl_uint) * bufVertexCount);
    cl::Buffer bufCosts   (context, CL_MEM_READ_WRITE, sizeof(cl_uint) * bufVertexCount);
    cl::Buffer bufNext    (context, CL_MEM_READ_WRITE, sizeof(cl_uint) * bufVertexCount);
    cl::Buffer bufPrevious(context, CL_MEM_READ_WRITE, sizeof(cl_uint) * bufVertexCount);

    cl::Kernel kernelInit(prog, "bfs_init");
    cl::Kernel kernelStageOne(prog, "bfs_stage");
    cl::Kernel kernelStageTwo(prog, "bfs_sync");

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
        kernelStageOne.setArg(5, bufNext);
        kernelStageOne.setArg(6, bufPrevious);
        kernelStageOne.setArg(7, bufVertexCount);
        kernelStageOne.setArg(8, bufEdgeCount);

        kernelStageTwo.setArg(0, bufVertex);
        kernelStageTwo.setArg(1, bufEdges);
        kernelStageTwo.setArg(2, bufFrontier);
        kernelStageTwo.setArg(3, bufVisited);
        kernelStageTwo.setArg(4, bufCosts);
        kernelStageTwo.setArg(5, bufNext);
        kernelStageTwo.setArg(6, bufPrevious);
        kernelStageTwo.setArg(7, bufVertexCount);
        kernelStageTwo.setArg(8, bufEdgeCount);
    }
    catch (cl::Error err)
    {
        ss << err.what() << std::endl;
        ss << err.err() << std::endl;
        throw;
    }

    //  initialize buffers on the GPU
    queue.enqueueNDRangeKernel(kernelInit, cl::NullRange, cl::NDRange(bufVertexCount));
    queue.finish();

    //  run BFS until finish
    //  calculate more than one stage each loop to keep the GPU busy
    ss << ">> GO\n";
    unsigned int i = 0;
    bool keepRunning = true;
    do 
    {
        auto ptrFrontier = (cl_uint*)queue.enqueueMapBuffer(bufFrontier, CL_TRUE, CL_MAP_READ, 0, sizeof(cl_uint) * bufVertexCount);
        auto frontSizeVorher = frontierSize(ptrFrontier, bufVertexCount);
        queue.enqueueUnmapMemObject(bufFrontier, ptrFrontier);
        queue.finish();

        queue.enqueueNDRangeKernel(kernelStageOne, cl::NullRange, cl::NDRange(bufVertexCount));
        queue.finish();
        queue.enqueueNDRangeKernel(kernelStageTwo, cl::NullRange, cl::NDRange(bufVertexCount));
        queue.finish();

        //  Map Frontier Buffer into host memory and check if it is empty
        //  If empty exit loop and print results, otherwise keep running
        ptrFrontier = (cl_uint*)queue.enqueueMapBuffer(bufFrontier, CL_TRUE, CL_MAP_READ, 0, sizeof(cl_uint) * bufVertexCount);
        auto frontSizeDanach = frontierSize(ptrFrontier, bufVertexCount);
        queue.enqueueUnmapMemObject(bufFrontier, ptrFrontier);

        ss << "Iteration #" << i << "; Vorher: " << frontSizeVorher << ", ";
        ss << "nachher " << frontSizeDanach << ", ";
        ss << static_cast<float>(frontSizeDanach) / frontSizeVorher << std::endl;
        ++i;
        keepRunning = (frontSizeDanach == 0) ? false : true;
    } while (keepRunning);
    ss << ">> FIN\n";

    auto ptrVisited = (cl_uint*)queue.enqueueMapBuffer(bufVisited, CL_TRUE, CL_MAP_READ, 0, sizeof(cl_uint) * bufVertexCount);
    auto visited = frontierSize(ptrVisited, bufVertexCount);
    queue.enqueueUnmapMemObject(bufVisited, ptrVisited);

    ss << visited << " visited, thats % " << (float(visited) / bufVertexCount) * 100 << std::endl;
    ss << "Loops needed: " << i << std::endl;
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
    const int vertexCount  = 50000000;
    const int edgesPerVert = 8;
    const int startVertex = rand() % vertexCount;
    const int endVertex = rand() % vertexCount;

    //  Print some info to the user while we create the graph
    cout << "Vertex count: " << vertexCount << ", (";
    cout << (sizeof(int) * vertexCount) / 1024 / 1024 << " MB)" << std::endl;
    cout << "Edge   count: " << vertexCount*edgesPerVert << ", (";
    cout << (sizeof(int) * vertexCount * edgesPerVert) / 1024 / 1024 << " MB)" << std::endl;

    //  Graphen erstellen
    cout << "Creating Graphs" << endl;
    GraphData graph(vertexCount, edgesPerVert, 1234);
    
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
        runBreadthFirstSearch(context, selectedDevice, graph, startVertex, endVertex, cout);
    }
    catch (cl::Error& err)
    {
        std::cerr << "Whoupdee*fucking*doo" << std::endl;
        std::cerr << err.what() << std::endl;
        std::cerr << err.err() << std::endl;
    }

    cin.get();
}
