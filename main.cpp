#include <iostream>
#include <string>
#include "HashMap.hpp"
#include "String.hpp"

#include <deque>
#include <iomanip>
#include <bitset>
#include <chrono>

using namespace std;

template<typename Func, typename... Args>
void timeit(Func func, Args&&... args){
	auto start = std::chrono::high_resolution_clock::now();
	func(std::forward<Args&&>(args)...);
	auto stop = std::chrono::high_resolution_clock::now();
	std::cout.precision(8);
	std::cout <<
		"Took: " << std::chrono::duration<double, std::ratio<1,1>>(stop - start).count()
		<< " seconds\n";
}

void __attribute__((optimize("O0"))) rgandomStdz(){
    for( int i = 0; i < 1'000'000; i++){
        std::string("This is small");
        std::string("This is very large, like, briouhahahrerete");
        std::string();
        std::string("This is small") = "Going to a very large buffer, I hope it doesnt as-if";
        std::string("This is small df eekjrekjnrkje") = "Go";
        std::string("small") == "Very very heavy stuff here ohfs";
        std::string("small dfkld ldfl lofm lkdf") == "fd";
        std::string("fd") == "fd";
    }
}

void __attribute__((optimize("O0"))) randomString(){
    for( int i = 0; i < 1'000'000; i++){
        FString("This is small");
        FString("This is very large, like, briouhahahrerete");
        FString();
        FString("This is small") = "Going to a very large buffer, I hope it doesnt as-if";
        FString("This is small df eekjrekjnrkje") = "Go";
        FString("small") == "Very very heavy stuff here ohfs";
        FString("small dfkld ldfl lofm lkdf") == "fd";
        FString("fd") == "fd";
    }
}

void __attribute__((optimize("O0"))) test_StdVector(){
    for( int i = 0; i < 1'000'000; i++){
        std::vector<FString> fv;
        fv.push_back(FString("This is small"));
        fv.push_back(FString("This is very large, like, briouhahahrerete"));
        fv.push_back(FString());
        fv.emplace_back(FString("This is small") = "Going to a very large buffer, I hope it doesnt as-if");
        fv.emplace_back("This is small df eekjrekjnrkje");
        auto mm = FString("small");
        FString mmn = "Very very heavy stuff here ohfs";
        fv.push_back(mm);
        fv.push_back(std::move(mm));
    }
}

void __attribute__((optimize("O0"))) test_FVector(){
    for( int i = 0; i < 1'000'000; i++){
        FVector<FString> fv;
        fv.push_back(FString("This is small"));
        fv.push_back(FString("This is very large, like, briouhahahrerete"));
        fv.push_back(FString());
        fv.emplace_back(FString("This is small") = "Going to a very large buffer, I hope it doesnt as-if");
        fv.emplace_back("This is small df eekjrekjnrkje");
        auto mm = FString("small");
        FString mmn = "Very very heavy stuff here ohfs";
        fv.push_back(mm);
        fv.push_back(std::move(mm));
    }
}

struct S{ char ch[24]; };

int main()
{

    cout << "Running CustomString....\n";
    timeit(randomString);
    cout << "\n---------------------\n";

    cout << "Running Std::String....\n";
    timeit(rgandomStdz);
    cout << "\n---------------------\n";

    cout << "Running FVector....\n";
    timeit(test_FVector);
    cout << "\n---------------------\n";

    cout << "Running std::vector....\n";
    timeit(test_StdVector);
    cout << "\n---------------------\n";

    FVector<FString> fv = {"This ", "better ", "be ", "good ", "hmmn " };
    for(int i=0; i<fv.size(); i++)
        cout << fv[i];
    cout << endl;

    cout << "Size of vector<S>: " << sizeof(FVector<char>) << " bytes" << endl;
    cout << "Size of Map<string, S>: " << sizeof(HashMap<std::string, S>) << " bytes" << endl;
/*
    String mm("This is epic");

    mm = "Are you kidding me?";

    cout << mm << endl;
    //cin >> mm;
    getline(std::cin, mm);
    bool fn = (mm == "Quite Funny");
    cout << "Is same? " << boolalpha << (mm == "Haha") << endl;

    int m = 34;
    int *p = new int[1024];
    std::bitset<64> mb(reinterpret_cast<unsigned long long>(&m));
    std::bitset<64> pb(reinterpret_cast<unsigned long long>(p));
    cout << "Address of m " << mb.to_ulong() << "  " << mb << endl;
    cout << "Address of p " << pb.to_ulong() << "  " << pb << endl;
    cout << "Size of SizeType: " << sizeof(SizeType) << " bytes" << endl;
    cout << "Size of String: " << sizeof(String) << " bytes" << endl;
    //cout << "Size of String::M: " << sizeof(String::Data) << " bytes" << endl;
    cout << "Size of std::string: " << sizeof(std::string) << " bytes" << endl;
    cout << "Size of std::forward_list<std::string>: " << sizeof(std::forward_list<std::string>) << " bytes" << endl;
    cout << "Size of S: " << sizeof(S) << " bytes" << endl;
    */
    return 0;
}
