
#include "GraphData.h"

#include <thread>
#include <future>
#include <iostream>


namespace
{
    const int WEIGHT_LIMIT = 60;
    const uint64_t NUM_THREADS = 4;
}


GraphData::GraphData(uint32_t vertexCount, uint32_t neighborsPerVertex, uint32_t seed)
    :m_neighborsPerVertex(neighborsPerVertex)
    ,m_vertices(vertexCount, 0)
    ,m_edges(static_cast<uint64_t>(vertexCount) * static_cast<uint64_t>(neighborsPerVertex), 0)
    ,m_weights(static_cast<uint64_t>(vertexCount) * static_cast<uint64_t>(neighborsPerVertex), 0)
{
    //  TODO: test if the edges and weights vectors can be _fully_ addressed
    //  with unsigned 32-bit integers
    uint64_t size = m_edges.size();
    if (size > UINT32_MAX)
        throw std::runtime_error("GraphData edges and weights can not be fully addressed\nGraph is too big");

    Initialize(seed);
}


//////////////////////////////////////////////////////////////////////////
GraphData::GraphData(const GraphData & rhs)
    :m_neighborsPerVertex(rhs.m_neighborsPerVertex)
    ,m_vertices(rhs.m_vertices)
    ,m_edges(rhs.m_edges)
    ,m_weights(rhs.m_weights)
{
    //  TODO: maybe print an info that the copy constructor got called
    //  because its f'n expensive?!
    std::cerr << "help pls, GraphData copy called!" << std::endl;
}


//////////////////////////////////////////////////////////////////////////
GraphData::GraphData(const GraphData&& rhs)
    :m_neighborsPerVertex(std::move(rhs.m_neighborsPerVertex))
    ,m_vertices(std::move(rhs.m_vertices))
    ,m_edges(std::move(rhs.m_edges))
    ,m_weights(std::move(rhs.m_weights))
{
}


//////////////////////////////////////////////////////////////////////////
uint64_t GraphData::VertexCount() const
{
    return m_vertices.size();
}
uint64_t GraphData::EdgeCount() const
{
    return m_edges.size();
}

uint32_t* GraphData::GetVertices()
{
    return m_vertices.data();
}
uint32_t* GraphData::GetEdges()
{
    return m_edges.data();
}
uint32_t* GraphData::GetWeights()
{
    return m_weights.data();
}

const uint32_t* GraphData::GetVertices() const
{
    return m_vertices.data();
}
const uint32_t* GraphData::GetEdges()    const
{
    return m_edges.data();
}
const uint32_t* GraphData::GetWeights()  const
{
    return m_weights.data();
}


//////////////////////////////////////////////////////////////////////////
bool GraphData::Equals(const GraphData & other) const
{
    //  pointer to the data arrays
    auto* vertices = GetVertices();
    auto* edges = GetEdges();
    auto* weights = GetWeights();

    //  pointers to data of right hand side
    auto* otherVert = other.GetVertices();
    auto* otherEdge = other.GetEdges();
    auto* otherWeig = other.GetWeights();

    if (other.VertexCount() != m_vertices.size())
        return false;

    if (other.EdgeCount() != m_edges.size())
        return false;

    //  check vertex data
    uint64_t vertexSize = m_vertices.size();
    for (uint64_t i = 0; i < vertexSize; ++i)
    {
        if (*vertices != *otherVert)
            return false;

        ++vertices;
        ++otherVert;
    }

    //  check edge and weight data
    uint64_t edgeSize = m_edges.size();
    for (uint64_t i = 0; i < edgeSize; ++i)
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

uint64_t GraphData::CalculateBytes() const
{
    uint64_t result;

    result += m_vertices.size();

    return result;
}

void GraphData::Initialize(uint32_t seed)
{
    const std::size_t vertexCount = m_vertices.size();
    const std::size_t worksize = vertexCount / NUM_THREADS;
    const std::size_t workLeft = vertexCount % NUM_THREADS;

    //  pointer to the data arrays
    auto* vertices = m_vertices.data();
    auto* edges = m_edges.data();
    auto* weights = m_weights.data();

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
            uint32_t vertexCount = static_cast<uint32_t>(m_vertices.size());

            for (uint64_t i = begin; i < end; ++i)
            {
                *(vertices + i) = static_cast<uint32_t>(i * m_neighborsPerVertex);
            }

            //  random generatore uses C++ Random Libs
            //  in this case, we use the mersenne twister engine
            //std::mt19937 rds;
            //std::mt19937 threadEngine(currentSeed);
            std::default_random_engine threadEngine(currentSeed);
            std::uniform_int_distribution<uint32_t> distrEdges(0, vertexCount);
            std::uniform_int_distribution<uint32_t> distrWeight(0, WEIGHT_LIMIT);
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


