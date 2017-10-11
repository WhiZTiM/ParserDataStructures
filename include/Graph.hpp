#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <limits>
#include <fstream>
#include <queue>

class EdgeIndex{
public:
    using IndexType = unsigned;
    constexpr operator IndexType() const { return m_value; }

    constexpr static EdgeIndex invalid()
    { return EdgeIndex(std::numeric_limits<IndexType>::max()); }

    EdgeIndex(EdgeIndex&& v) : m_value(v.m_value) {}
    EdgeIndex(const EdgeIndex& v) : m_value(v.m_value) {}
    EdgeIndex& operator = (EdgeIndex&& v) { m_value = v.m_value; return *this; }
    EdgeIndex& operator = (const EdgeIndex& v) { m_value = v.m_value; return *this; }

private:
    template<typename, typename>
    friend class Graph;

    template<typename, typename>
    friend class DiGraph;

    constexpr explicit EdgeIndex(IndexType value) : m_value(value) {}
    IndexType m_value;
};

bool operator == (const EdgeIndex& x, const EdgeIndex& y)
{ return static_cast<EdgeIndex::IndexType>(x) == static_cast<EdgeIndex::IndexType>(y); }

bool operator != (const EdgeIndex& x, const EdgeIndex& y)
{ return !(x == y); }







class VertexIndex{
public:
    using IndexType = unsigned;
    constexpr operator IndexType() const { return m_value; }

    constexpr static VertexIndex invalid()
    { return VertexIndex(std::numeric_limits<IndexType>::max()); }

    VertexIndex(VertexIndex&& v) : m_value(v.m_value) {}
    VertexIndex(const VertexIndex& v) : m_value(v.m_value) {}
    VertexIndex& operator = (VertexIndex&& v) { m_value = v.m_value; return *this; }
    VertexIndex& operator = (const VertexIndex& v) { m_value = v.m_value; return *this; }
private:
    template<typename, typename>
    friend class Graph;

    template<typename, typename>
    friend class DiGraph;

    constexpr explicit VertexIndex(IndexType value) : m_value(value) {}

    IndexType m_value;
};


bool operator == (const VertexIndex& x, const VertexIndex& y)
{ return static_cast<VertexIndex::IndexType>(x) == static_cast<VertexIndex::IndexType>(y); }

bool operator != (const VertexIndex& x, const VertexIndex& y)
{ return !(x == y); }










template<typename First, typename Second>
class Bimap{
public:
    using UMapA = std::unordered_map<First, Second>;
    using UMapB = std::unordered_map<Second, typename UMapA::iterator>;

    void insert(const First& first, const Second& second){
        m_second_to_first[second] = m_first_to_second.emplace(first, second).first;
    }

    const First& first_from_second(const Second& s) const {
        return m_second_to_first.find(s)->second->first;
    }

    const Second& second_from_first(const First& f) const {
        return m_first_to_second.find(f)->second;
    }

    bool exists_in_first(const First& f) const {
        return m_first_to_second.count(f);
    }

    bool exists_in_second(const Second& s) const {
        return m_second_to_first.count(s);
    }

    int size() const { return m_first_to_second.size(); }

private:
    UMapA m_first_to_second;
    UMapB m_second_to_first;
};










template<typename Identifier>
class Vertex{
public:
    using IdentifierType = Identifier;
public:
    Vertex(Identifier&& id) : m_identifier(std::move(id)) {}
    Vertex(const Identifier& id) : m_identifier(id) {}

    Identifier& identifier(){ return m_identifier; }
    const Identifier& identifier() const { return m_identifier; }

protected:
    Identifier m_identifier;
};

template<typename Identifier>
bool operator == (const Vertex<Identifier>& a, const Vertex<Identifier>& b)
{   return a.identifier() == b.identifier();  }










template<typename Weight_Type = int>
class Edge{
public:
    using IdentifierType = int;
    using WeightType = Weight_Type;
public:
    Edge(WeightType weight) : m_weight(weight) {}

    static WeightType min() { return std::numeric_limits<WeightType>::lowest(); }
    static WeightType max() { return std::numeric_limits<WeightType>::max(); }

    static WeightType positive_infinity() {
        if(std::numeric_limits<WeightType>::has_infinity)
            return std::numeric_limits<WeightType>::infinity();
        return max();
    }

    WeightType weight() const { return m_weight; }
    IdentifierType identifier() const { return m_weight; }

private:
    WeightType m_weight;
    IdentifierType m_id;

    void set_identifier(IdentifierType id){ m_id = id; }

    template<typename, typename>
    friend class DiGraph;

    template<typename, typename>
    friend class Graph;
};

template<typename Weight>
bool operator == (const Edge<Weight>& a, const Edge<Weight>& b)
{  return a.identifier() == b.identifier();  }






namespace std {

    template<typename T>
    struct hash<Vertex<T>>{
        std::size_t operator() (const Vertex<T>& v) const
        { return std::hash<typename Vertex<T>::IdentifierType>()(v.identifier()); }
    };

    template<typename T>
    struct hash<Edge<T>>{
        std::size_t operator() (const Edge<T>& v) const
        { return std::hash<typename Edge<T>::IdentifierType>()(v.identifier()); }
    };

    template<>
    struct hash<EdgeIndex>{
        std::size_t operator () (const EdgeIndex& e) const
        { return std::hash<EdgeIndex::IndexType>()(static_cast<EdgeIndex::IndexType>(e)); }
    };

    template<>
    struct hash<VertexIndex>{
        std::size_t operator () (const VertexIndex& e) const
        { return std::hash<VertexIndex::IndexType>()(static_cast<VertexIndex::IndexType>(e)); }
    };
}







template<typename Vertex_Type, typename Edge_Type>
class Graph {
public:
    struct AdjacencyElement{
        EdgeIndex   edge_index;
        VertexIndex vertex_index;
    };
    using EdgeType = Edge_Type;
    using EdgeWeightType = typename EdgeType::WeightType;
    using VertexType = Vertex_Type;
    using AdjacencyType = std::vector<AdjacencyElement>;

public:
    void add_vertex(VertexType vt){
        m_vertexMap.insert(vt, VertexIndex(m_vertexMap.size()));
        m_adjacencyList.emplace_back();
    }

    void connect(VertexType source, VertexType destination){
        connect(source, destination, EdgeType(1));
    }

    void connect(VertexType source, VertexType destination, EdgeType edge){
        auto src = m_vertexMap.second_from_first(source);
        auto dest = m_vertexMap.second_from_first(destination);

        edge.set_identifier(m_edgeLookup.size());
        EdgeIndex index(m_edgeLookup.size());
        m_edgeMap.insert(edge, index);
        m_edgeLookup.emplace_back(src, dest);
        m_adjacencyList[src].push_back({index, dest});
        m_adjacencyList[dest].push_back({index, src});
    }

    const AdjacencyType& outbound_links_from(const VertexType& vertex) const {
        return outbound_links_from(index_from_vertex(vertex));
    }

    const AdjacencyType& outbound_links_from(const VertexIndex& index) const {
        if(m_adjacencyList[index].empty())
            return empty_adjacency_object;
        return m_adjacencyList[index];
    }

    VertexType get_vertex(VertexIndex index) const {
        return m_vertexMap.first_from_second(index);
    }

    EdgeType get_edge(EdgeIndex index) const {
        return m_edgeMap.first_from_second(index);
    }

    VertexIndex get_vertex_index(const VertexType& vertex) const {
        return m_vertexMap.second_from_first(vertex);
    }

    EdgeIndex get_edge_index(const EdgeType& edge) const {
        return m_edgeMap.second_from_first(edge);
    }

    bool exists(const EdgeType& edge) const {
        return m_edgeMap.exists_in_first(edge);
    }

    bool exists(const VertexType& vertex) const {
        return m_vertexMap.exists_in_first(vertex);
    }

    int vertex_size() const { return m_adjacencyList.size(); }

private:
    Bimap<EdgeType, EdgeIndex>                          m_edgeMap;
    Bimap<VertexType, VertexIndex>                      m_vertexMap;
    std::vector<AdjacencyType>                          m_adjacencyList;
    std::vector<std::pair<VertexIndex, VertexIndex>>    m_edgeLookup;
    int                                                 m_edgeCounter = 0;

    static AdjacencyType empty_adjacency_object;
private:
};

template<typename A, typename B>
typename Graph<A, B>::AdjacencyType Graph<A,B>::empty_adjacency_object = {};



template<class GraphType>
class DijkstraShortestPath{

    using Vertex = typename GraphType::VertexType;
    using Edge = typename GraphType::EdgeType;
    using WeightType = typename GraphType::EdgeWeightType;

    struct DistFunc{
        DistFunc(const std::vector<WeightType>& dist) : distances(dist) {}
        bool operator ()(const VertexIndex& x, const VertexIndex& y) const {
            return distances[x] > distances[y];
        }
        const std::vector<WeightType>& distances;
    };

public:
    DijkstraShortestPath(const GraphType& graph_, const Vertex& source_)
        : graph(graph_),
          source(graph.get_vertex_index(source_)),
          visited(graph.vertex_size(), false),
          distances(graph.vertex_size(), Edge::positive_infinity()),
          backtrack(graph.vertex_size(), {EdgeIndex::invalid(), VertexIndex::invalid()}),
          queue(DistFunc(distances))
    {
        distances[source] = 0;
        queue.push(source);
    }

    WeightType shortest_path_to(const Vertex& destination_vertex){
        const VertexIndex dest = graph.get_vertex_index(destination_vertex);
        while (!queue.empty()) {
            const VertexIndex v = queue.top();
            queue.pop();
            if(v == dest)
                break;
            if(visited[v])
                continue;
            visited[v] = true;
            for(auto ve : graph.outbound_links_from(v)){
                auto edge_weight = graph.get_edge(ve.edge_index).weight();
                auto local_cost = distances[v] + edge_weight;
                if(local_cost < distances[ve.vertex_index]){
                    distances[ve.vertex_index] = local_cost;
                    backtrack[ve.vertex_index] = ve;
                }
                queue.push(ve.vertex_index);
            }
        }
        return distances[dest];
    }


private:
    const GraphType& graph;
    const VertexIndex source;
    std::vector<bool>       visited;
    std::vector<WeightType> distances;
public:
    std::vector<typename GraphType::AdjacencyElement> backtrack;
private:
    std::priority_queue<VertexIndex, std::vector<VertexIndex>, DistFunc> queue;

};

template<typename GraphType = Graph<Vertex<std::string>, Edge<>>>
GraphType createGraph_fromFile(const std::string& filename){
    GraphType graph;
    using Vertex = typename GraphType::VertexType;
    using Edge = typename GraphType::EdgeType;
    std::ifstream fp(filename);
    int iterMax; fp >> iterMax;
    for(int x = 0; x < iterMax; x++){
        std::string v;
        fp >> v;
        graph.add_vertex(Vertex(v));
    }
    fp >> iterMax;
    for(int x = 0; x < iterMax; x++){
        std::string v1, v2;
        int weight;
        fp >> v1 >> v2 >> weight;
        graph.connect(Vertex(v1), Vertex(v2), Edge(weight));
    }
    return graph;
}
#endif // GRAPH_HPP
