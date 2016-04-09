
#include "MemoryAllocator.hpp"
#include "catch.hpp"
#include <string>
#include <vector>

TEST_CASE( "Memory Allocator", "[memory]" ){
    PoolAllocator<int> mem;
    std::vector<decltype(mem.allocate())> ptrs;

    for(int i = 0; i < 112; i++){
        ptrs.push_back(mem.allocate());
    }

    for(unsigned i = 0; i < ptrs.size(); i++){
        std::cout << "allocation " << i << ": " << ptrs[i] << std::endl;
    }

    for(auto& x : ptrs)
        mem.deallocate(x);

    //REQUIRE(  )
}

