#ifndef MEMORYALLOCATOR_HPP
#define MEMORYALLOCATOR_HPP

#include <new>
#include <deque>
#include <vector>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <iostream>

/*!
 *
 * \note BlockSize must be multiples of 8
 */
template<typename T, unsigned short BlockSize = 16>
class PoolAllocator{
    struct MemoryBlock;

public:

    using Container = std::vector<MemoryBlock>;

    PoolAllocator(){
        freeblocks.emplace_back();
    }

    T* allocate(){
        auto& bk = freeblocks[lastFreeIndex];
        T* ptr = bk.try_allocate();
        if(ptr)
            return ptr;
        //else its full, remove it and add new pool;
        usedblocks.emplace_back(std::move(bk));
        freeblocks.erase( freeblocks.begin() + lastFreeIndex);

        if(freeblocks.size() < 1)       //Add new Block
            freeblocks.emplace_back();
        return freeblocks[lastFreeIndex].try_allocate();
    }

    void deallocate(T* ptr){
        //Check freeblocks first, higher chances user will deallocate soon
        auto iter1 = std::find_if(freeblocks.begin(), freeblocks.end(),
                                 [ptr](const MemoryBlock& c){ return c.contains(ptr); } );

        if(iter1 != freeblocks.end())
            iter1->deallocate(ptr);
        else{
            auto iter2 = std::find_if(usedblocks.begin(), usedblocks.end(),
                                      [ptr](const MemoryBlock& c){ return c.contains(ptr); } );

            if(iter2 != usedblocks.end()){
                iter2->deallocate(ptr);
                freeblocks.emplace_back(std::move(*iter2));
                usedblocks.erase(iter2);
                return;
            }
        }
    }

private:


    struct MemoryBlock{
        T* data;
        bool allocatedBlocks[BlockSize];

        //! allocates memory block for 'sizeof(T) * BlockSize'
        MemoryBlock() {
            std::memset(allocatedBlocks, false, sizeof(bool)*BlockSize);
            data = static_cast<T*>( ::operator new ( sizeof(T) * BlockSize ) );
        }
        MemoryBlock(MemoryBlock&& other){
            data = other.data;
            other.data = nullptr;
            std::memcpy(allocatedBlocks, other.allocatedBlocks, sizeof(bool)*BlockSize);
        }
        MemoryBlock& operator = (MemoryBlock&& other){
            ::operator delete (data);
            data = other.data;
            other.data = nullptr;
            std::memcpy(allocatedBlocks, other.allocatedBlocks, sizeof(bool)*BlockSize);
            return *this;
        }

        MemoryBlock(const MemoryBlock&) = delete;
        MemoryBlock& operator = (const MemoryBlock&) = delete;

        ~MemoryBlock(){
            ::operator delete (data);
        }

        inline bool is_full() const {
            // Fastest way I know to test for truth in a boolean array
            return  (allocatedBlocks[0] == true) &&
                    (std::memcmp(allocatedBlocks, allocatedBlocks+1, sizeof(bool)*(BlockSize - 1)));
        }

        T* try_allocate() {
            for(unsigned short i=0; i < BlockSize; i++)
                // This relies on && short circuiting properties. see ISO C++14 ($5.14.1)
                // if a block isn't marked allocated, it marks it and returns the block
                if((!allocatedBlocks[i]) && (allocatedBlocks[i] = true))
                    return data + i;
            return nullptr;
        }
        inline void deallocate(T* ptr){
            allocatedBlocks[(data - ptr)];
        }

        bool contains(T* ptr) const {
            // see ... http://stackoverflow.com/a/3692961/1621391
            assert( ((data - ptr) % sizeof(T) == 0) && " ptr is not suitably aligned!" );
            return (data <= ptr && ptr < (data + BlockSize) ) ;
        }
    };
    std::size_t lastFreeIndex = 0;
    Container freeblocks;
    Container usedblocks;
};


#endif // MEMORYALLOCATOR_HPP

