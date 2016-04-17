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

TEST_CASE( "vectors can be sized and resized", "[vector]" ) {

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

TEST_CASE( "vectors can be reserved at construction", "[vector]" ) {

    FVector<int> v( 5, FVector<int>::reserve_tag );

    REQUIRE( v.size() == 0 );
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

        REQUIRE( v.size() == 0 );
        REQUIRE( v.capacity() >= 10 );
    }
    SECTION( "reserving smaller does not change size or capacity" ) {
        v.reserve( 0 );

        REQUIRE( v.size() == 0 );
        REQUIRE( v.capacity() >= 5 );
    }
}

TEST_CASE( "list initialization of vectors", "[vector]" ) {

    FVector<int> v{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    REQUIRE( v.size() == 10 );
    REQUIRE( v.capacity() >= 10 );

    SECTION( "resizing bigger changes size and capacity" ) {
        v.resize( 14 );

        REQUIRE( v.size() == 14 );
        REQUIRE( v.capacity() >= 14 );
    }
    SECTION( "resizing smaller changes size but not capacity" ) {
        v.resize( 0 );

        REQUIRE( v.size() == 0 );
        REQUIRE( v.capacity() >= 10 );
    }
    SECTION( "reserving bigger changes capacity but not size" ) {
        v.reserve( 16 );

        REQUIRE( v.size() == 10 );
        REQUIRE( v.capacity() >= 16 );
    }
    SECTION( "reserving smaller does not change size or capacity" ) {
        v.reserve( 0 );

        REQUIRE( v.size() == 10 );
        REQUIRE( v.capacity() >= 10 );
    }
    SECTION( "Comparing it with same std::vector element "){
        std::vector<int> x{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        REQUIRE( std::equal(std::begin(x), std::end(x), std::begin(v), std::end(v)) );

        v.emplace_back({11, 12, 13, 14, 15, 16, 17});
        x.insert(x.end(), v.begin()+10, v.end());

        REQUIRE( std::equal(std::begin(x), std::end(x), std::begin(v), std::end(v)) );
    }
    SECTION( "Reassign it "){
        std::vector<int> x{ 3, 2, 7, 4, 9, 8, 10, 5, 6, 1 };

        v.clear();
        std::copy(x.begin(), x.end(), std::back_inserter(v));
        REQUIRE( std::equal(std::begin(x), std::end(x), std::begin(v), std::end(v)) );

        std::sort(v.begin(), v.end());
        std::sort(x.begin(), x.end());

        REQUIRE( std::equal(std::begin(x), std::end(x), std::begin(v), std::end(v)) );
    }
    SECTION( "Using some STL algorithm should work well "){
        std::vector<int> x(v.begin(), v.end());

        std::transform(v.begin(), v.end(), v.begin(), std::bind(std::multiplies<>(), std::placeholders::_1, 5));
        std::transform(x.begin(), x.end(), x.begin(), std::bind(std::multiplies<>(), std::placeholders::_1, 5));

        REQUIRE( std::equal(std::begin(x), std::end(x), std::begin(v), std::end(v)) );

        //std::copy(v.cbegin(), v.cend(), std::ostream_iterator<int>(std::cout, " "));

        auto q1 = std::accumulate(v.cbegin(), v.cend(), 500);
        auto q2 = std::accumulate(x.cbegin(), x.cend(), 500);
        REQUIRE( q1 == q2);
    }

}

TEST_CASE( "erasing elements works", "[vector]" ) {

    FVector<int> v{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };

    REQUIRE( v.size() == 13 );
    REQUIRE( v.capacity() >= 13 );

    SECTION( "erasing a single element at the begining" ) {
        REQUIRE( v.size() == 13 );
        std::vector<int> x(v.begin(), v.end());
        REQUIRE( x.size() == 13 );

        v.erase(v.begin());
        x.erase(x.begin());

        REQUIRE( x.size() == 12 );
        REQUIRE( v.size() == 12 );
        REQUIRE( v.capacity() >= 13 );
    }
    SECTION( "erasing a single element at the end" ) {

        REQUIRE( v.size() == 13 );
        std::vector<int> x(v.begin(), std::prev(v.end()));
        REQUIRE( x.size() == 12 );

        v.erase(std::prev(v.end()));

        REQUIRE( v.size() == 12 );
        REQUIRE( std::equal(std::begin(x), std::end(x), std::begin(v), std::end(v)) );
    }
    SECTION( "erasing a single element at the middle" ) {

        std::vector<int> x(v.begin(), v.end());
        REQUIRE( x.size() == 13 );

        v.erase(v.begin() + 4);
        x.erase(x.begin() + 4);

        v.erase(v.end() - 3);
        x.erase(x.end() - 3);

        REQUIRE( v.size() == 11 );
        REQUIRE( std::equal(std::begin(x), std::end(x), std::begin(v), std::end(v)) );
    }
    SECTION( "erasing a range works" ) {

        std::vector<int> x(v.begin(), v.end());
        REQUIRE( x.size() == 13 );

        auto vx1 = std::find(v.begin(), v.end(), 5);
        auto vx2 = std::find(v.begin(), v.end(), 10);
        auto xx1 = std::find(x.begin(), x.end(), 5);
        auto xx2 = std::find(x.begin(), x.end(), 10);
        v.erase(vx1, vx2);
        x.erase(xx1, xx2);

        REQUIRE( v.size() == 8 );
        REQUIRE( x.size() == 8 );
        REQUIRE( std::equal(std::begin(x), std::end(x), std::begin(v), std::end(v)) );
    }
}
