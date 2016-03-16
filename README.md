# ParserDataStructures
Fast and lightweight STL-like containers designed to be used by parsers. (requires C++14)

This is a header-only library of STL-like containers designed to outperform STL containers in parsing applications in terms of speed and memory overhead. And unlike STL, you can absolutely use incomplete types in these containers

Motivation
==========

I designed a modern C++14 stream parsing library with a variant type. I did this relying completely on STL. I had 3 notable problems:

* `std::string`'s implementation across different STL implementations had significant variations in performance and memory overhead for short strings, sometimes, these performance discrepancies are unacceptable. The `FString` implementation here is at least 10% faster than GCC 5.3.x implementations and at least 20% faster than MSVC (Visual Studio) 2015 implementation. And its a maximum of `sizeof(FString) == 24 bytes` on any platform. Using Short String Optimization of 16 characters... Meanwhile `sizeof(std::string) == 40 bytes` in VS2015 64bit, and  `sizeof(std::string) == 32 bytes` in GCC 5.3.x 64bit

* `std::vector` requires complete types, hence its illegal and Undefined Behaviour to have a member `std::vector<Type>` in `class Type`. The best you could do is to use an indirection. The same goes with other containers such as `std::unordered_map<T>`, etc.

* `std::vector`'s size overhead varied across different implementations, on most STL implementations its 32bytes or larger. I needed something lower. And I reduced usage by using `32bit` integers (~4billion) in my implementations. You rarely have a single data structure use up to that much items in most cases.

* reducing the size alone increased performance for a vector small items, up to 20% in benchmarks

**NOTE:** There are a few rarely used features in STL that were stripped off. The Use of allocators here is of `static thread_local`, all containers here do not store their allocators.
