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


typedef struct  
{
    unsigned int  vertexCount;
    unsigned int* vertexArray;

    unsigned int  edgeCount;
    unsigned int* edgeArray;

    unsigned int* weightArray;
} GraphData ;


//  helpers
int selectClPlatform(std::vector<cl::Platform> platforms);
cl::Device selectClDevice(cl::Platform& platform);
std::string loadFileContent(std::string filename);

bool frontierEmpty(const cl_int* frontier, unsigned int vertexCount);
void generateGraph(GraphData* data, int vertexCount, int neighborsPerVertex);

//  run the program
void runBreadthFirstSearch(GraphData& data);
void runDijkstra(int argc, char* argv[]);

#endif   /* ----- #ifndef PAP_DIJKSTRA_H  ----- */
