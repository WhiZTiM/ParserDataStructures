
#include "MemoryAllocator.hpp"
#include "catch.hpp"
#include <string>
#include <vector>

class CX {
public:
    CX(int iA, std::string sA, double dA)
        : i(iA), s(sA), d(dA) {}
    CX(const CX&) = delete;

    double getD() const { return d; }

private:
    int i;
    std::string s;
    double d;
};

TEST_CASE( "Memory Allocator", "[memory]" ){
    PoolAllocator<int> mem_int;
    PoolAllocator<CX> mem_string;
    std::vector<decltype(mem_int.allocate())> mem_int_ptrs;
    std::vector<decltype(mem_string.allocate())> mem_string_ptrs;

    for(int i = 0; i < 112; i++){
        mem_int_ptrs.push_back(mem_int.allocate());
        mem_string_ptrs.push_back(mem_string.construct(i, "Hola! Mucho Gusto!", i * i));
    }

    for(unsigned i = 0; i < mem_int_ptrs.size(); i++){
        std::cout << "int    _allocation " << i << ": " << mem_int_ptrs[i] << std::endl;
        std::cout << "string _allocation " << i << ": " << mem_string_ptrs[i]->getD() << std::endl;
    }

    for(auto& x : mem_int_ptrs)
        mem_int.deallocate(x, true);
    for(auto x : mem_string_ptrs)
        mem_string.destruct_and_deallocate(x, true);

    //REQUIRE(  )
}

