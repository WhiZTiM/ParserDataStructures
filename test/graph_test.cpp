/*
* ParserDataStructures
*
* Distributed under the Boost Software License, Version 1.0.
* (See accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* Author: Ibrahim Timothy Onogu
* Email: ionogu@acm.org
* Project Date: March, 2016
*/

//#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>
#include "FVector.hpp"
#include "Graph.hpp"

TEST_CASE( "Graphs", "[graph]" ) {

    using VertexT = Vertex<std::string>;
    using EdgeT = Edge<std::size_t>;
    using GraphT = Graph<VertexT, EdgeT>;

    GraphT dg;
    dg.add_vertex(VertexT("Bauchi"));
    dg.add_vertex(VertexT("Plateau"));
    dg.add_vertex(VertexT("Gombe"));
    dg.add_vertex(VertexT("Abuja"));

    dg.connect( VertexT("Bauchi"), VertexT("Gombe"), EdgeT(165) );
    dg.connect( VertexT("Bauchi"), VertexT("Plateau"), EdgeT(125) );

    auto graph = createGraph_fromFile<>("/home/whiztim/Documents/developer/Python/DataStructure/graphInput.txt");

    DijkstraShortestPath<decltype(graph)> dj(graph, VertexT("Gombe"));

    std::cout << "Size of Graph " << sizeof(decltype(dj)) << std::endl;

    std::cout << "Testing: " << dj.shortest_path_to(VertexT("Abuja")) << std::endl;
//    auto start = graph.get_vertex_index(VertexT("Gombe"));
//    auto next = dj.backtrack[graph.get_vertex_index(VertexT("Abuja"))];
//    while(next.vertex_index != stop){
//        std::cout << "From: " << graph.get_vertex()
//    }

    FVector<int> v( 5 );

    REQUIRE( v.size() == 5 );
    REQUIRE( v.capacity() >= 5 );

    SECTION( "resizing bigger changes size and capacity" ) {
        v.resize( 10 );

        REQUIRE( v.size() == 10 );
        REQUIRE( v.capacity() >= 10 );
    }
    SECTION( "resizing smaller changes size but not capacity" ) {
        v.resize( 0 );

        REQUIRE( v.size() == 0 );
        REQUIRE( v.capacity() >= 5 );
    }
    SECTION( "reserving bigger changes capacity but not size" ) {
        v.reserve( 10 );

        REQUIRE( v.size() == 5 );
        REQUIRE( v.capacity() >= 10 );
    }
    SECTION( "reserving smaller does not change size or capacity" ) {
        v.reserve( 0 );

        REQUIRE( v.size() == 5 );
        REQUIRE( v.capacity() >= 5 );
    }
}
