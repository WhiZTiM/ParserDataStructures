
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
#include <FVector.hpp>
#include "HashMap.hpp"
#include "String.hpp"
#include <string>

TEST_CASE( "HashMaps should work", "[hash_map]" ) {

    FVector<int> v( 5 );

    using Str = std::string;
    //using Str = FString;

    HashMap<FString, int> mp;

    mp.insert(std::pair<Str, int>("Haha", 23));
    mp.insert(std::pair<Str, int>("Huhu", 283));
    mp.insert(std::pair<Str, int>("Yaha", 5234));
    mp.insert(std::pair<Str, int>("Maha", -823));
    mp.insert(std::pair<Str, int>("Kaha", -2423));
    mp.insert(std::pair<Str, int>("Vaha", -9993));
    mp.insert(std::pair<Str, int>("loki", -1113));

    cout << "\n\n\t\tNOWWW\n\n";
    for(auto x : mp)
        cout << "[" << x.first << ": " << x.second << ']' << endl;

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
