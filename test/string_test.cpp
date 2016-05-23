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
//#include <typeinfo>
#include <string>
#include "String.hpp"

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
        REQUIRE(str.size() == 12 );

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

        //std::cout << typeid(ppx).name() << std::endl;

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
