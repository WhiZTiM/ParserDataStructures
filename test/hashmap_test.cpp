
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

    //using Str = std::string;
    using Str = FString;

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


    SECTION( "All iterator elements exists" ){
        auto iter = mp.begin();
        REQUIRE( mp.find((*iter++).first) != mp.end() );
        REQUIRE( mp.find((*iter++).first) != mp.end() );
        REQUIRE( mp.find((*iter++).first) != mp.end() );
        REQUIRE( mp.find((*iter++).first) != mp.end() );
        REQUIRE( mp.find((*iter++).first) != mp.end() );
        REQUIRE( mp.find((*iter++).first) != mp.end() );
        REQUIRE( mp.find((*iter++).first) != mp.end() );
    }

    SECTION( "All keys Exists "){
        REQUIRE( mp.find("Haha") != mp.end() );
        REQUIRE( (*mp.find("Haha")).first == "Haha" );
        REQUIRE( (*mp.find("Haha")).second == 23 );

        REQUIRE( mp.find("Huhu") != mp.end() );
        REQUIRE( (*mp.find("Huhu")).first == "Huhu" );
        REQUIRE( (*mp.find("Huhu")).second == 283 );

        REQUIRE( mp.find("Yaha") != mp.end() );
        REQUIRE( (*mp.find("Yaha")).first == "Yaha" );
        REQUIRE( (*mp.find("Yaha")).second == 5234 );

        REQUIRE( mp.find("Maha") != mp.end() );
        REQUIRE( (*mp.find("Maha")).first == "Maha" );
        REQUIRE( (*mp.find("Maha")).second == -823 );

        REQUIRE( mp.find("Kaha") != mp.end() );
        REQUIRE( (*mp.find("Kaha")).first == "Kaha" );
        REQUIRE( (*mp.find("Kaha")).second == -2423 );

        REQUIRE( mp.find("Vaha") != mp.end() );
        REQUIRE( (*mp.find("Vaha")).first == "Vaha" );
        REQUIRE( (*mp.find("Vaha")).second == -9993 );

        REQUIRE( mp.find("loki") != mp.end() );
        REQUIRE( (*mp.find("loki")).first == "loki" );
        REQUIRE( (*mp.find("loki")).second == -1113 );
    }

}
