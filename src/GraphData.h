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

    std::size_t VertexCount()   { return m_vertices.size(); }
    std::size_t EdgeCount()     { return m_edges.size(); }

    std::size_t VertexCount() const { return m_vertices.size(); }
    std::size_t EdgeCount()   const { return m_edges.size(); }

    int* GetVertices()  { return m_vertices.data(); }
    int* GetEdges()     { return m_edges.data(); }
    int* GetWeights()   { return m_weights.data(); }

    const int* GetVertices() const { return m_vertices.data(); }
    const int* GetEdges()    const { return m_edges.data(); }
    const int* GetWeights()  const { return m_weights.data(); }

    bool Equals(const GraphData& other) const;

private:
    void Initialize(uint32_t seed);

    const int m_neighborsPerVertex;
    std::vector<int> m_vertices;
    std::vector<int> m_edges;
    std::vector<int> m_weights;
};


#endif // GraphData_h__
