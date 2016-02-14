/*
* =====================================================================================
*
*       Filename:  pap_bfs.hpp
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


#ifndef  PAP_BFS_H
#define  PAP_BFS_H

#include "cppcl.hpp"
#include "GraphData.h"


//  helpers
std::string loadFileContent(std::string filename);

bool frontierEmpty(const cl_int* frontier, int vertexCount);
int frontierSize (const cl_int* frontier, int vertexCount);

//  run the program
void runBreadthFirstSearch(cl::Context& context,
                           cl::Device& device,
                           GraphData& data,
                           uint32_t startVertex,
                           uint32_t endVertex,
                           std::ostream& ss,
                           const uint16_t innerLoops = 1,
                           const uint16_t workgroupSize = 32);

#endif   /* ----- #ifndef PAP_BFS_H  ----- */
