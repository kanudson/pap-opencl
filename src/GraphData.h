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

    int* GetVertices();
    int* GetEdges();
    int* GetWeights();
    const int* GetVertices() const;
    const int* GetEdges()    const;
    const int* GetWeights()  const;

    bool Equals(const GraphData& other) const;

private:
    void Initialize(uint32_t seed);

    const int m_neighborsPerVertex;
    std::vector<int> m_vertices;
    std::vector<int> m_edges;
    std::vector<int> m_weights;
};


#endif // GraphData_h__
