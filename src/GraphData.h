#ifndef GraphData_h__
#define GraphData_h__


#include <random>
#include <vector>

class GraphData
{
public:
    //  Initializes a graph with a possible seed
    GraphData(uint32_t vertexCount, uint32_t neighborsPerVertex, uint32_t seed = 0);

    //  Copies the graph data
    GraphData(const GraphData& rhs);

    GraphData(const GraphData&& rhs);
    GraphData& operator= (const GraphData& data) = delete;
    GraphData& operator= (const GraphData&& data) = delete;

    uint64_t VertexCount()  const;
    uint64_t EdgeCount()    const;

    uint32_t* GetVertices();
    uint32_t* GetEdges();
    const uint32_t* GetVertices() const;
    const uint32_t* GetEdges()    const;

    bool Equals(const GraphData& other) const;

private:
    void Initialize(uint32_t seed);

    const int m_neighborsPerVertex;
    std::vector<uint32_t> m_vertices;
    std::vector<uint32_t> m_edges;
};


#endif // GraphData_h__
