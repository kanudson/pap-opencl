
///
/// This is part 1 of the Kernel from Algorithm 4 in the paper
///
kernel void bfs_stage(global read_only int *vertexArray, global read_only int *edgeArray,
                          global int *frontierArray, global int *visitedArray,
                          global int *costArray,
                          global int *previousVertexArray,
                          int vertexCount, int edgeCount)
{
    int tid = get_global_id(0);

    //  only process the vertex when it's in the frontier
    if (frontierArray[tid] != 0)
    {
        //  remove vertex from frontier
        frontierArray[tid] = 0;
        visitedArray[tid]  = 1;

        int edgeStart = vertexArray[tid];
        int edgeEnd;
        if (tid + 1 < (vertexCount))
            edgeEnd = vertexArray[tid + 1];
        else
            edgeEnd = edgeCount;

        for (int edge = edgeStart; edge < edgeEnd; ++edge)
        {
            //  check if vertex behin edge was visited
            int nextVertexId = edgeArray[edge];

            if (visitedArray[nextVertexId] == 0)
            {
                atomic_xchg(&costArray[nextVertexId], costArray[tid] + 1);
                atomic_xchg(&frontierArray[nextVertexId], 1);
                atomic_xchg(&previousVertexArray[nextVertexId], tid);
            }
        }
    }
}


// Kernel to initialize buffers
//
kernel void bfs_init(global int *frontierArray, global int *visitedArray,
                     global int *costArray,
                     global int *previousVertexArray,
                     int vertexCount, unsigned int sourceVertex)
{
    unsigned int tid = get_global_id(0);

    if (sourceVertex == tid)
    {
        frontierArray[tid] = 1;
        costArray[tid] = 0;
        visitedArray[tid] = 0;
        previousVertexArray[tid] = tid;
    }
    else
    {
        frontierArray[tid] = 0;
        costArray[tid] = 0;
        visitedArray[tid] = 0;
        previousVertexArray[tid] = tid;
    }
}

