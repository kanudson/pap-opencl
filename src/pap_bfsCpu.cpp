
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

#include "pap_bfs.h"
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

    //  logging variables
    uint64_t frontierBefore = 0;
    uint64_t frontierAfter = 0;

    //  puffer
    std::vector<uint32_t> frontier;
    std::vector<uint32_t> toVisitNext;
    std::vector<uint32_t> visited(vertexCount, 0);

    for (auto& v : visited)
        v = 0;

    //  start vertex eintragen
    frontier.push_back(startVertex);
    uint32_t i = 0;
    while (frontier.size() > 0)
    {
        frontierBefore = frontier.size();
        toVisitNext.clear();

        uint32_t edgeStart;
        uint32_t edgeEnd;
        //  iterate throug complete frontier array
        for (uint32_t currentVertexID : frontier)
        {
            visited[currentVertexID] = 2;

            edgeStart = vertices[currentVertexID];
            if (currentVertexID >= vertexCount)
                edgeEnd = edgeCount;
            else
                edgeEnd = vertices[currentVertexID + 1];

            //  iterate over all edges
            for (uint32_t edge = edgeStart; edge < edgeEnd; ++edge)
            {
                uint32_t nextVertex = edges[edge];
                if (visited[nextVertex] != 1)
                {
                    toVisitNext.push_back(nextVertex);
                }
            }
        }

        for (uint32_t vertexID : frontier)
        {
            visited[vertexID] = 1;
        }

        //  clear frontier afterwards and fill the
        //  selected vertices that should be visited next
        frontier = std::move(toVisitNext);
        frontierAfter = frontier.size();
        
        uint32_t visitCount = 0;
        for (auto& vertex : visited)
        {
            if (vertex == 1)
                ++visitCount;
        }
        ss << "Iteration #" << i++ << "; before: " << frontierBefore << ", after " << frontierAfter << " (" << visitCount << ")\n";
    }
}
