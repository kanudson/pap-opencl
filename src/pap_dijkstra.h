/*
* =====================================================================================
*
*       Filename:  pap_dijkstra.hpp
*
*    Description:
*
*        Version:  1.0
*        Created:  10/15/2015 10:21:05 PM
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Paul Thieme
*   Organization:
*
* =====================================================================================
*/


#ifndef  PAP_DIJKSTRA_H
#define  PAP_DIJKSTRA_H


#include "cppcl.hpp"
#include "GraphData.h"


//  helpers
int selectClPlatform(std::vector<cl::Platform> platforms);
cl::Device selectClDevice(cl::Platform& platform);
std::string loadFileContent(std::string filename);

bool frontierEmpty(const cl_int* frontier, int vertexCount);
int frontierSize (const cl_int* frontier, int vertexCount);

//  run the program
void runBreadthFirstSearch(cl::Context& context,
                           cl::Device& device,
                           GraphData& data,
                           uint32_t startVertex,
                           uint32_t endVertex);
void runDijkstra(int argc, char* argv[]);

#endif   /* ----- #ifndef PAP_DIJKSTRA_H  ----- */
