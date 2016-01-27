
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
    ,m_edges(vertexCount * neighborsPerVertex, 0)
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

const uint32_t* GraphData::GetVertices() const
{
    return m_vertices.data();
}
const uint32_t* GraphData::GetEdges()    const
{
    return m_edges.data();
}

//////////////////////////////////////////////////////////////////////////
bool GraphData::Equals(const GraphData & other) const
{
    //  pointer to the data arrays
    auto* vertices = GetVertices();
    auto* edges = GetEdges();

    //  pointers to data of right hand side
    auto* otherVert = other.GetVertices();
    auto* otherEdge = other.GetEdges();

    //  check if sizes differ
    if (other.VertexCount() != m_vertices.size() ||
        other.EdgeCount() != m_edges.size())
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

    //  check edge data
    uint64_t edgeSize = m_edges.size();
    for (uint64_t i = 0; i < edgeSize; ++i)
    {
        if (*edges != *otherEdge)
            return false;

        ++edges;
        ++otherEdge;
    }

    return true;
}

void InitializeGraphChunk(uint32_t* vertecies, uint32_t* edges, const uint32_t offset, const uint32_t chunkSize, const uint32_t vertexCount, const uint32_t neighbors, const uint32_t seed)
{
    //  initialize vertecies
    for (uint32_t i = 0; i < chunkSize; ++i)
    {
        *vertecies = (i + offset) * neighbors;
        ++vertecies;
    }

    //  initialize edge data
    //  amount = edgeCount * neighborsPerVertex
    std::default_random_engine rd(seed);
    std::uniform_int_distribution<uint32_t> distrEdges(0, vertexCount);
    for (uint32_t i = 0; i < (chunkSize * neighbors); ++i)
    {
        *edges = distrEdges(rd);
        ++edges;
    }
}

void GraphData::Initialize(uint32_t seed)
{
    const std::size_t vertexCount = m_vertices.size();
    const std::size_t worksize = vertexCount / NUM_THREADS;
    const std::size_t workLeft = vertexCount % NUM_THREADS;

    //  pointer to the data arrays
    auto* vertices = m_vertices.data();
    auto* edges = m_edges.data();

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
        auto chunkSize = end - begin;

        //  initialize data in own thread
        std::thread runner(InitializeGraphChunk, vertices + begin, edges + begin,
                           begin, chunkSize, vertexCount,
                           m_neighborsPerVertex, currentSeed);
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


