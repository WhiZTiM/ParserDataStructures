
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
#include <unordered_map>
#include "HashMap.hpp"
#include "String.hpp"
#include <string>

TEST_CASE( "HashMaps should work", "[hash_map]" ) {

    using Str = std::string;
    //using Str = FString;

    HashMap<FString, int> mp;

    REQUIRE( mp.size() == 0 );

    mp.insert(std::pair<Str, int>("Haha", 23));
    mp.insert(std::pair<Str, int>("Huhu", 283));
    mp.insert(std::pair<Str, int>("Yaha", 5234));
    mp.insert(std::pair<Str, int>("Maha", -823));
    mp.insert(std::pair<Str, int>("Kaha", -2423));
    mp.insert(std::pair<Str, int>("Vaha", -9993));
    mp.insert(std::pair<Str, int>("loki", -1113));

    const SizeType mpSize = 7;

    REQUIRE( mp.size() == mpSize );
    //cout << "\n\n\t\tNOWWW\n\n";
    //for(auto x : mp)
    //    cout << "[" << x.first << ": " << x.second << ']' << endl;


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
    SECTION( " Erase works "){
        auto x = mp.find("Haha");
        REQUIRE( x != mp.end() );

        mp.erase(x);
        auto y = mp.find("Haha");
        REQUIRE( y == mp.end() );
        REQUIRE( mp.size() == mpSize - 1 );

        REQUIRE( mp.find("Vaha") != mp.end() );
        mp.erase("Vaha");
        REQUIRE( mp.find("Vaha") == mp.end() );
        REQUIRE( mp.size() == mpSize - 2 );

        mp.erase("Vaha");
        REQUIRE( mp.find("Vaha") == mp.end() );

    }
    SECTION(" Erasing or extracting works"){

        mp["Sani"] = 435;
        mp["Mako"] = 435;
        mp["Tim"] = 435;
        mp["Sani"] = 435;
        REQUIRE(mp.size() == mpSize + 3);


        REQUIRE(mp.erase("Sani") == 1);
        REQUIRE(mp.size() == mpSize + 2);

        auto nxt = mp.find("Mako")++;   //next iterator after "Mako"
        REQUIRE(mp.erase(mp.find("Mako")) == nxt ); //assert erase returns the next iterator
        REQUIRE(mp.size() == mpSize + 1 );

        REQUIRE(mp.find("Tim") != mp.end());
        auto node = mp.extract("Tim");
        REQUIRE(mp.find("Tim") == mp.end());
        REQUIRE(mp.size() == mpSize);
    }

    SECTION( "Test extraction and merging" ){
        mp["Sani"] = 435;
        mp["Mako"] = 435;
        mp["Tim"] = 435;
        mp["Sani"] = 435;
        REQUIRE(mp.size() == mpSize + 3);

        auto sani = mp.extract("Sani");
        REQUIRE(mp.find("Sani") == mp.end());
        REQUIRE(mp.size() == mpSize + 2);
        REQUIRE(sani.is_empty() == false);

        auto tim = mp.extract(mp.find("Tim"));
        REQUIRE(mp.find("Tim") == mp.end());
        REQUIRE(mp.size() == mpSize + 1);
        REQUIRE(tim.is_empty() == false);

        auto mako = mp.extract(FString("Mako"));
        REQUIRE(mp.find("Mako") == mp.end());
        REQUIRE(mp.size() == mpSize);
        REQUIRE(mako.is_empty() == false);

        auto james = mp.extract("Hullabaloo");
        REQUIRE(mp.find("Hullabaloo") == mp.end());
        REQUIRE(mp.size() == mpSize);
        REQUIRE(james.is_empty() == true);

        ////////  INSERTION   ////////
        auto sI = mp.insert(std::move(sani));
        REQUIRE(sI.inserted == true);
        REQUIRE(sI.node.is_empty() == true);
        REQUIRE(sI.position == mp.find("Sani"));
        REQUIRE(mp.size() == mpSize + 1);

        mp["Tim"] = 789;
        auto tI = mp.insert(std::move(tim));
        REQUIRE(tI.inserted == false);
        REQUIRE(tI.node.is_empty() == false);
        REQUIRE(tI.position == mp.find("Tim"));
        REQUIRE(mp.size() == mpSize + 2);

        auto mI = mp.insert(std::move(mako));
        REQUIRE(mI.inserted == true);
        REQUIRE(mI.node.is_empty() == true);
        REQUIRE(mI.position != mp.find("Tim"));
        REQUIRE(mI.position == mp.find("Mako"));
        REQUIRE(mp.size() == mpSize + 3);

        auto jI = mp.insert(std::move(james));
        REQUIRE(jI.inserted == false);
        REQUIRE(jI.node.is_empty() == true);
        REQUIRE(jI.position == mp.end());
    }

    SECTION( "Return value of insert works "){
        auto x = mp.insert(std::make_pair(Str("lol"), 6541));   // "lol" is a new key
        auto y = mp.insert(std::make_pair(Str("loki"), 65414)); // "loki was already inserted earlier

        REQUIRE( x.first == mp.find("lol") );
        REQUIRE( x.second == true );
        REQUIRE( mp.size() == mpSize + 1 );

        REQUIRE( y.first == mp.find("loki") );
        REQUIRE( y.second == false );

        REQUIRE( x.first != y.first );
    }
    SECTION( "operator[] should have the same semantics as std::unordered_map "){
        mp["lol"] = 87;
        REQUIRE( mp.size() == mpSize + 1 );

        auto x = mp.insert(std::make_pair(Str("lol"), 65414));
        //auto x = mp.emplace("lol", 65414);
        REQUIRE( x.first == mp.find("lol") );   // iterators are the same
        REQUIRE( x.second == false );           // it did not insert
        REQUIRE( (*x.first).second == 87 );     // value is the same
        REQUIRE( mp.size() == mpSize + 1 );

        REQUIRE( mp.find("loki") != mp.end() );                 // "loki" exists
        auto loki_value = (*mp.find("loki")).second;            // get its value;
        REQUIRE( loki_value != 99 );                             // not same as the new value we gonna put

        //try reassign "loki"
        mp["loki"] = 99;
        auto new_value = (*mp.find("loki")).second;
        REQUIRE( new_value != loki_value );
        REQUIRE( new_value == 99 );
        REQUIRE( mp.size() == mpSize + 1 );
    }
    SECTION("Compare with std::unordered_map"){
        auto hash = [](const FString& fs){ return std::hash<std::string>()(fs.to_string()); };
        auto equal = [](const FString& a, const FString& b){ return a == b; };
        std::unordered_map<FString, int, decltype(hash), decltype(equal)> xp(19, hash, equal);

        /*
        xp.insert(std::pair<Str, int>("Haha", 23));
        xp.insert(std::pair<Str, int>("Huhu", 283));
        xp.insert(std::pair<Str, int>("Yaha", 5234));
        xp.insert(std::pair<Str, int>("Maha", -823));
        xp.insert(std::pair<Str, int>("Kaha", -2423));
        xp.insert(std::pair<Str, int>("Vaha", -9993));
        xp.insert(std::pair<Str, int>("loki", -1113));
        */

        for(auto& x : mp)
            xp.insert(x);

        REQUIRE( std::all_of(mp.cbegin(), mp.cend(), [&](auto x){ return xp.find(x.first) != xp.end(); }) );

    }

    SECTION("Swap And Copy Construction"){
        auto kd = mp;
        REQUIRE( std::all_of(mp.cbegin(), mp.cend(), [&](auto x){ return kd.find(x.first) != kd.end(); }) );
        decltype(mp) pl;
        pl.emplace("sweet", 111);

        std::swap(pl, kd);
        REQUIRE( std::all_of(mp.cbegin(), mp.cend(), [&](auto x){ return pl.find(x.first) != pl.end(); }) );

        auto ql = std::move(pl);

        REQUIRE( pl.empty() );
        REQUIRE( pl.size() == 0 );
        REQUIRE( std::all_of(mp.cbegin(), mp.cend(), [&](auto x){ return ql.find(x.first) != ql.end(); }) );
    }

}
