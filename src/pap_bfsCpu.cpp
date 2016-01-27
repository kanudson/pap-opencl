
/*
* =====================================================================================
*
*       Filename:  pap_BfsCpu.cpp
*
*    Description:
*
*        Version:  1.0
*        Created:  01/27/2016 03:32:36 PM
*       Revision:  none
*       Compiler:  vc++
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

void runBfsCpu(GraphData& graph, uint32_t startVertex, std::ostream& ss)
{
    uint32_t vertexCount = graph.VertexCount();
    uint32_t edgeCount  = graph.EdgeCount();
    uint32_t* vertices  = graph.GetVertices();
    uint32_t* edges     = graph.GetEdges();

    //  puffer
    std::vector<uint32_t> frontier;
    std::vector<uint32_t> toVisitNext;
    std::vector<uint32_t> visited(vertexCount, 0);
    std::vector<uint32_t> costs(vertexCount, 0);

    //  start vertex eintragen
    frontier.push_back(startVertex);
    uint32_t i = 0;
    while (!frontier.empty())
    {
        uint32_t edgeStart;
        uint32_t edgeEnd;
        //  iterate throug complete frontier array
        for (uint32_t vertex : frontier)
        {
            visited[vertex] = 1;

            edgeStart = vertices[vertex];
            if (vertex >= vertexCount)
                edgeEnd = edgeCount;
            else
                edgeEnd = vertices[vertex + 1];

            //  iterate over all edges
            for (uint32_t edge = edgeStart; edge < edgeEnd; ++edge)
            {
                uint32_t nextVertex = edges[edge];
                uint32_t currentCost = costs[vertex];
                if (visited[nextVertex] == 0)
                {
                    costs[nextVertex] = currentCost + 1;
                    toVisitNext.push_back(nextVertex);
                }
            }
        }

        //  clear frontier afterwards and fill the
        //  selected vertices that should be visited next
        frontier = std::move(toVisitNext);
        ss << "Iteration " << i++ << " done\n";
    }
}
