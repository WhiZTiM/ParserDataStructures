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
//#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <vector>
#include <algorithm>
#include <string>
#include "String.hpp"
#include <sstream>

unsigned int Factorial( unsigned int number ) {
    return number <= 1 ? number : Factorial(number-1)*number;
}

TEST_CASE( "Factorials are computed", "[factorial]" ) {
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );
}


TEST_CASE( "Test Construction and Assingment of Small Strings", "[string]" ) {

    FString str;
    std::vector<int> v( 5 );

    REQUIRE( str.size() == 0 );
    REQUIRE( v.size() == 5 );
    REQUIRE( v.capacity() >= 5 );

    SECTION( "Assigning empty string to 'Hello World!' should be 12 characters" ){

        str = "Hello World!";
        REQUIRE(str.size() == 12);
        REQUIRE(str[0] == 'H');     // First character
        REQUIRE(str[11] == '!');    // Last  character
        REQUIRE(str[12] == '\0');   // Null terminator
    }

    SECTION( "Modifying the string shouldn't be a problem" ){

        str = "Hello World!";

        REQUIRE(str[0]  == 'H');
        REQUIRE(str[5]  == ' ');
        REQUIRE(str[11] == '!');

        str[0] =  'Y';
        str[5] =  '^';
        str[11] = '$';

        REQUIRE(str[0]  == 'Y');
        REQUIRE(str[5]  == '^');
        REQUIRE(str[11] == '$');

        REQUIRE(str == "Yello^World$");
    }

    SECTION( "Assigning empty string to std::string('Hello World!') should be 12 characters" ){

        str = std::string("Hello World!");
        REQUIRE(str.size() == 12);
        REQUIRE(str[0] == 'H');     // First character
        REQUIRE(str[11] == '!');    // Last  character
        REQUIRE(str[12] == '\0');   // Null terminator

        auto s = FString(std::string("Hello World!"));
        REQUIRE(str == s);
    }

    SECTION( "Move assigning Shouldn't be a problem" ){

        str = FString("Hello World!");
        REQUIRE(str.size() == 12);
        REQUIRE(str[0] == 'H');     // First character
        REQUIRE(str[11] == '!');    // Last  character
        REQUIRE(str[12] == '\0');   // Null terminator

        auto s = FString(std::string("Hello World!"));
        REQUIRE(str == s);
    }

    SECTION( "Copy assigning Shouldn't be a problem" ){

        auto sx = FString("Hello World!");
        str = sx;
        REQUIRE(str.size() == 12);
        REQUIRE(str[0] == 'H');     // First character
        REQUIRE(str[11] == '!');    // Last  character
        REQUIRE(str[12] == '\0');   // Null terminator

        auto s = FString(std::string("Hello World!"));
        REQUIRE(str == s);
        REQUIRE(str == sx);
    }

    SECTION( "Constructing from std::string('Hello World!') should be 12 characters" ){

        //Name shadowing of str
        FString str = std::string("Hello World!");

        REQUIRE(str.size() == 12);
        REQUIRE(str[0] == 'H');     // First character
        REQUIRE(str[11] == '!');    // Last  character
        REQUIRE(str[12] == '\0');   // Null terminator

        auto s = FString(std::string("Hello World!"));
        REQUIRE(str == s);
    }
    SECTION( "Test comparison operators" ){
        str = "aa";
        auto str2 = str;
        str2[1] = 'b';

        REQUIRE(str < str2);
        REQUIRE(str2 > str);
        REQUIRE(str <= str2);
        REQUIRE(str != str2);
        REQUIRE(str2 >= str);
    }
}

TEST_CASE( "Test Construction and Assingment of Large Strings", "[string]" ) {

    FString str = "The quick brown fox jumps over the lazy dog";

    REQUIRE( str.size() == 43  );
    REQUIRE( str[0]     == 'T' );
    REQUIRE( str[str.size()]        == '\0' );
    REQUIRE( str[str.size() - 1]    == 'g'  );

    SECTION( "strlen works on them..."){
        REQUIRE( std::strlen(str.data()) == 43 );
        REQUIRE( std::strlen(str.c_str()) == 43 );
    }

    SECTION( "Assigning to short string to 'Hello World!' should be 12 characters" ){

        str = "Hello World!";
        REQUIRE(str.size() == 12);
        REQUIRE(str[0] == 'H');     // First character
        REQUIRE(str[11] == '!');    // Last  character
        REQUIRE(str[12] == '\0');   // Null terminator

        REQUIRE(std::strlen(str.data()) == 12);
        REQUIRE(std::strlen(str.c_str()) == 12);

        const auto& myRef = str;

        REQUIRE(std::strlen(myRef.data()) == 12);
        REQUIRE(std::strlen(myRef.c_str()) == 12);
    }

    SECTION( "Modifying the string shouldn't be a problem" ){

        str[0] =  'Y';
        str[5] =  '^';
        str[11] = '$';

        str[str.size() - 1] = 'k';

        REQUIRE(str[0]  == 'Y');
        REQUIRE(str[5]  == '^');
        REQUIRE(str[11] == '$');
        REQUIRE(str[str.size() - 1] == 'k');

        REQUIRE(str == "Yhe q^ick b$own fox jumps over the lazy dok");
    }

    SECTION( "Assigning empty string to std::string('Hello World!') should be 12 characters" ){

        str = std::string("Hello World!");
        REQUIRE(str.size() == 12);
        REQUIRE(str[0] == 'H');     // First character
        REQUIRE(str[11] == '!');    // Last  character
        REQUIRE(str[12] == '\0');   // Null terminator

        auto s = FString(std::string("Hello World!"));
        REQUIRE(str == s);
    }

    SECTION( "Copy Constructing Shouldn't be a problem" ){

        str = FString("Hello World!");
        REQUIRE(str.size() == 12);
        REQUIRE(str[0] == 'H');     // First character
        REQUIRE(str[11] == '!');    // Last  character
        REQUIRE(str[12] == '\0');   // Null terminator

        auto s = FString(std::string("Hello World!"));
        REQUIRE(str == s);
    }

    SECTION( "Constructing from std::string('Hello World!') should be 12 characters" ){

        //Name shadowing of str
        FString str = std::string("Hello World!");

        REQUIRE(str.size() == 12);
        REQUIRE(str[0] == 'H');     // First character
        REQUIRE(str[11] == '!');    // Last  character
        REQUIRE(str[12] == '\0');   // Null terminator

        auto s = FString(std::string("Hello World!"));
        REQUIRE(str == s);
    }

    SECTION( "Constructing from const char* ('Hello World!') should be 12 characters" ){

        //Name shadowing of str
        const char* ppx = "Hello World!";

        FString str(ppx);

        REQUIRE(str.size() == 12);
        REQUIRE(str[0] == 'H');     // First character
        REQUIRE(str[11] == '!');    // Last  character
        REQUIRE(str[12] == '\0');   // Null terminator

        auto s = FString(std::string("Hello World!"));
        REQUIRE(str == s);
    }
}

TEST_CASE("Using STL algorithm is acceptable", "[string]"){

    FString str = "The quick brown fox jumps over the lazy dog";
    std::string str2 = "The quick brown fox jumps over the lazy dog";

    REQUIRE(str.size() == str2.size());

    SECTION("Non modifying algorithm"){
        REQUIRE(std::equal(std::begin(str), std::end(str), std::begin(str2), std::end(str2)));

        std::copy(str2.cbegin(), str2.cend(), str.begin());
        REQUIRE(std::equal(std::begin(str), std::end(str), std::begin(str2), std::end(str2)));
    }
    SECTION("Sorting algorithm"){
        std::sort(str.begin(), str.end());
        std::sort(str2.begin(), str2.end());
        REQUIRE( std::equal(std::begin(str), std::end(str), std::begin(str2), std::end(str2)) );
    }
}

TEST_CASE("Using Iterators", "[string]"){
    FString str = "The quick brown fox jumps over the lazy dog";
    std::string str2 = "The quick brown fox jumps over the lazy dog";

    SECTION("Forward Iterators"){
        REQUIRE( std::equal(std::begin(str), std::end(str), std::begin(str2), std::end(str2)) );
    }

    SECTION("Reverse Iterators"){
        std::reverse(str.begin(), str.end());

        REQUIRE( std::equal(str.crbegin(), str.crend(), std::begin(str2), std::end(str2)) );
    }
}

TEST_CASE("Find and SubStr Methods", "[string]"){
    FString str = "the quick brown fox jumps over the lazy dog";
    std::string str2 = "the quick brown fox jumps over the lazy dog";

    SECTION("Test Find Forward"){
        auto x1 = str.find("jumps");
        auto x2 = str.find("Mumu");

        auto y1 = str2.find("jumps");

        REQUIRE( x1 != FString::npos );
        REQUIRE( x2 == FString::npos );

        REQUIRE( x1 == y1 );

        REQUIRE( str.find('o') == str2.find('o') );
        REQUIRE( str.find('0') == FString::npos  );
        REQUIRE( str.find('u', 27) == FString::npos );
        REQUIRE( str.find("jumps", 26) == FString::npos );

        REQUIRE( str.find('o', 17) == str2.find('o', 17) );
        REQUIRE( str.find("the", 17) == str2.find("the", 17) );
    }

    SECTION("Test Find backwards"){
        auto x1 = str.find("the");
        auto x2 = str.rfind("the");

        auto y1 = str2.find("the");
        auto y2 = str2.rfind("the");

        REQUIRE( x1 == y1 );
        REQUIRE( x2 == y2 );

        REQUIRE( str.rfind('o') == str2.rfind('o') );
        REQUIRE( str.rfind('o', 29) == str2.rfind('o', 29) );

        REQUIRE( str.rfind("the", 19) == str2.rfind("the", 19) );

        REQUIRE( str.rfind("Mumu") == FString::npos );
        REQUIRE( str.rfind('0')    == FString::npos );

        REQUIRE( str.rfind('o', 9) == FString::npos );
    }

    SECTION("Test substr method"){

        //small string
        auto substr = FString("Today").substr(2);
        REQUIRE( substr == "day" );

        substr = FString("Today").substr(2, 3);
        REQUIRE( substr == "d" );

        //Large String
        substr = str.substr(8);
        auto xubstr = str2.substr(8);

        REQUIRE( substr.to_string() == xubstr );

        substr = str.substr(10, 37);
        xubstr = str2.substr(10, 37);

        REQUIRE( substr.to_string() == xubstr );
    }

    SECTION("Swap Works"){
        FString ks = str;
        FString ms = "Hullabaloo";
        swap(ms, str);

        REQUIRE( str == "Hullabaloo" );
        REQUIRE( ms == ks );
    }

}

TEST_CASE("Test with streams", "[string]"){
    FString str = "This is so amazing";

    SECTION("getline test"){

        std::string s;
        std::stringstream ss("This better be a good thing");
        while (ss >> str) {
            s += str.to_string();
        }

        REQUIRE( s == "Thisbetterbeagoodthing" );
    }
}
