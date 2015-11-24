#ifndef GraphData_h__
#define GraphData_h__


#include <vector>
#include <random>


class GraphData
{
public:
    //  Initializes a graph with a possible seed
    GraphData(uint32_t vertexCount, uint32_t neighborsPerVertex, uint32_t seed = 0);

    //  Copies the graph data
    GraphData(const GraphData& rhs);

    GraphData(const GraphData&& rhs) = delete;
    GraphData& operator= (const GraphData& data) = delete;

    uint64_t VertexCount()  const;
    uint64_t EdgeCount()    const;

    uint32_t* GetVertices();
    uint32_t* GetEdges();
    uint32_t* GetWeights();
    const uint32_t* GetVertices() const;
    const uint32_t* GetEdges()    const;
    const uint32_t* GetWeights()  const;

    bool Equals(const GraphData& other) const;
    uint64_t CalculateBytes() const;

private:
    void Initialize(uint32_t seed);

    const int m_neighborsPerVertex;
    std::vector<uint32_t> m_vertices;
    std::vector<uint32_t> m_edges;
    std::vector<uint32_t> m_weights;
};


#endif // GraphData_h__
