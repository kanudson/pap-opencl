
#include "GraphData.h"

#include <thread>
#include <future>


namespace
{
    const int WEIGHT_LIMIT = 60;
    const uint64_t NUM_THREADS = 4;
}


GraphData::GraphData(uint32_t vertexCount, uint32_t neighborsPerVertex, uint32_t seed)
    :m_neighborsPerVertex(neighborsPerVertex)
    ,m_vertices(vertexCount, 0)
    ,m_edges(vertexCount * neighborsPerVertex, 0)
    ,m_weights(vertexCount * neighborsPerVertex, 0)
{
    Initialize(seed);
}


bool GraphData::Equals(const GraphData & other) const
{
    //  pointer to the data arrays
    const int* vertices = GetVertices();
    const int* edges = GetEdges();
    const int* weights = GetWeights();

    //  pointers to data of right hand side
    const int* otherVert = other.GetVertices();
    const int* otherEdge = other.GetEdges();
    const int* otherWeig = other.GetWeights();

    if (other.VertexCount() != m_vertices.size())
        return false;

    if (other.EdgeCount() != m_edges.size())
        return false;

    uint32_t vertexSize = m_vertices.size();
    for (uint32_t i = 0; i < vertexSize; ++i)
    {
        if (*vertices != *otherVert)
            return false;

        ++vertices;
        ++otherVert;
    }

    uint32_t edgeSize = m_edges.size();
    for (uint32_t i = 0; i < edgeSize; ++i)
    {
        if (*edges != *otherEdge)
            return false;

        if (*weights != *otherWeig)
            return false;

        ++edges;
        ++otherEdge;
        ++weights;
        ++otherWeig;
    }

    return true;
}

void GraphData::Initialize(uint32_t seed)
{
    const std::size_t vertexCount = m_vertices.size();
    const std::size_t worksize = vertexCount / NUM_THREADS;
    const std::size_t workLeft = vertexCount % NUM_THREADS;

    //  pointer to the data arrays
    int* vertices = m_vertices.data();
    int* edges = m_edges.data();
    int* weights = m_weights.data();

    //  seed generator for the different generators in each thread
    std::mt19937 engine(seed);

    uint64_t begin = 0;
    uint64_t end = worksize;
    std::vector<std::thread> threads;
    for (uint64_t i = 0; i < NUM_THREADS; ++i)
    {
        if (i == (NUM_THREADS - 1))
            end += workLeft;

        uint32_t currentSeed = engine();

        std::thread runner([=]()
        {
            std::size_t vertexCount = m_vertices.size();

            for (uint32_t i = begin; i < end; ++i)
            {
                *(vertices + i) = i * m_neighborsPerVertex;
            }

            //  random generatore uses C++ Random Libs
            //  in this case, we use the mersenne twister engine
            //std::mt19937 rds;
            std::mt19937 threadEngine(currentSeed);
            std::uniform_int_distribution<int> distrEdges(0, vertexCount);
            std::uniform_int_distribution<int> distrWeight(0, WEIGHT_LIMIT);
            const uint64_t edgeEnd = end * m_neighborsPerVertex;
            for (uint64_t i = begin; i < edgeEnd; ++i)
            {
                *(edges + i)= distrEdges(threadEngine);
                *(weights + i)= distrWeight(threadEngine) + 1;
            }
        });
        threads.push_back(std::move(runner));

        begin = end;
        end += worksize;
    }

    //  rejoin all the threads
    for (auto& runner : threads)
    {
        runner.join();
    }
}


