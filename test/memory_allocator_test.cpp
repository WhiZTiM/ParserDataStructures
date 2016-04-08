
#include "MemoryAllocator.hpp"
#include "catch.hpp"
#include <string>

TEST_CASE( "Memory Allocator", "[memory]" ){
    PoolAllocator<std::string> mem;
    std::vector<decltype(mem.allocate())> ptrs;

    for(int i = 0; i < 21; i++){
        ptrs.push_back(mem.allocate());
    }

    for(unsigned i = 0; i < ptrs.size(); i++){
        std::cout << "allocation " << i << ": " << ptrs[i] << std::endl;
    }
    //REQUIRE(  )
}

