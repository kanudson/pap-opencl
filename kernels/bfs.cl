
///
/// This is part 1 of the Kernel from Algorithm 4 in the paper
///
kernel void bfs_stage(global read_only uint* vertexArray, global read_only uint* edgeArray,
                          global uint* frontierArray, global uint* visitedArray,
                          global uint* costArray, global uint* toVisitNext,
                          global uint* previousVertexArray,
                          ulong vertexCount, ulong edgeCount)
{
    ulong tid = get_global_id(0);

    if (tid > vertexCount)
        return;

    //  only process the vertex when it's in the frontier
    private uint doStuff = frontierArray[tid];
    if (doStuff != 0)
    {
        //  remove vertex from frontier
        frontierArray[tid] = 0;
        visitedArray[tid]  = 1;

        __private uint edgeStart = vertexArray[tid];
        __private uint edgeEnd;
        if (tid + 1 < (vertexCount))
            edgeEnd = vertexArray[tid + 1];
        else
            edgeEnd = edgeCount;

        for (uint edge = edgeStart; edge < edgeEnd; ++edge)
        {
            //  check if vertex behin edge was visited
            __private uint nextVertexId = edgeArray[edge];
            __private uint currentCosts = costArray[tid];

            if (visitedArray[nextVertexId] == 0)
            {
                costArray[nextVertexId] = currentCosts + 1;
                toVisitNext[nextVertexId] = 1;
                //previousVertexArray[nextVertexId] = tid;
            }
        }
    }
}


kernel void bfs_sync(global read_only uint* vertexArray, global read_only uint* edgeArray,
                          global uint* frontierArray, global uint* visitedArray,
                          global uint* costArray, global uint* toVisitNext,
                          global uint* previousVertexArray,
                          ulong vertexCount, ulong edgeCount)
{
    ulong tid = get_global_id(0);

    if (tid > vertexCount)
        return;

    if (toVisitNext[tid] != 0)
    {
        frontierArray[tid] = 1;
        toVisitNext[tid] = 0;
    }
}


// Kernel to initialize buffers
//
kernel void bfs_init(global uint* frontierArray, global uint* visitedArray,
                     global uint* costArray,
                     global uint* previousVertexArray,
                     ulong vertexCount, uint sourceVertex)
{
    uint tid = get_global_id(0);

    if (tid >= vertexCount)
        return;

    if (sourceVertex == tid)
    {
        frontierArray[tid] = 1;
        costArray[tid] = 0;
        visitedArray[tid] = 0;
        //previousVertexArray[tid] = tid;
    }
    else
    {
        frontierArray[tid] = 0;
        costArray[tid] = 0;
        visitedArray[tid] = 0;
        //previousVertexArray[tid] = tid;
    }
}

