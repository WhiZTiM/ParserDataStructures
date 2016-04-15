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

#ifndef MEMORYALLOCATOR_HPP
#define MEMORYALLOCATOR_HPP

#include <new>
#include <deque>
#include <vector>
#include <cstring>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <Config.hpp>
#include <type_traits>

/*!
 *
 * \note BlockSize must be multiples of 8
 */
template<typename T, unsigned short BlockSize = 16>
class PoolAllocator{

    template<typename U> inline
    std::enable_if_t<std::is_class<U>::value, void>
    FORCE_INLINE call_destructor(U* t){
        t->~U();
    }

    template<typename U> inline
    std::enable_if_t<!std::is_class<U>::value, void>
    FORCE_INLINE call_destructor(U*){}

public:
    struct MemoryBlock;

public:

    using Container = std::vector<MemoryBlock>;

    PoolAllocator(){
        freeblocks.emplace_back();
    }

    template<typename... Args>
    T* construct(Args... args){
        T* t = allocate();
        new (t) T(std::forward<Args>(args)...);
        return t;
    }

    void destruct_and_deallocate(T* t, bool compressMemory = false){
        call_destructor(t);
        deallocate(t, compressMemory);
    }

    T* allocate(){
        auto& bk = freeblocks.back();
        T* ptr = bk.try_allocate();
        if(ptr)
            return ptr;
        //else its full, remove it and add new pool;
        usedblocks.emplace_back(std::move(bk));
        freeblocks.pop_back();

        if(freeblocks.size() < 1)       //Add new Block
            freeblocks.emplace_back();
        return freeblocks.back().try_allocate();
    }

    void deallocate(T* ptr, bool compressMemory = false){
        //Check freeblocks first, higher chances user will deallocate soon
        auto iter1 = std::find_if(freeblocks.begin(), freeblocks.end(),
                                 [ptr](const MemoryBlock& c){ return c.contains(ptr); } );

        if(iter1 != freeblocks.end()){
            iter1->deallocate(ptr);
            if(compressMemory && iter1->is_empty() ){
                freeblocks.erase(iter1);
            }
        }
        else{
            auto iter2 = std::find_if(usedblocks.begin(), usedblocks.end(),
                                      [ptr](const MemoryBlock& c){ return c.contains(ptr); } );

            if(iter2 != usedblocks.end()){
                iter2->deallocate(ptr);
                freeblocks.emplace_back(std::move(*iter2));
                usedblocks.erase(iter2);
            }
        }
    }

public:


    struct MemoryBlock{
        T* data;
        bool allocatedBlocks[BlockSize];

        //! allocates memory block for 'sizeof(T) * BlockSize'
        MemoryBlock() {
            std::memset(allocatedBlocks, false, sizeof(bool)*BlockSize);
            data = static_cast<T*>( ::operator new ( sizeof(T) * BlockSize ) );
            assert((reinterpret_cast<std::uintptr_t>(data) % alignof(T) == 0) &&
                   " Allocator failed to get a suitably aligned memory!" );
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
                    (0 == std::memcmp(allocatedBlocks, allocatedBlocks+1, sizeof(bool)*(BlockSize - 1)));
        }

        inline bool is_empty() const {
            // Fastest way I know to test for falsehood in a boolean array
            return  (allocatedBlocks[0] == false) &&
                    (0 == std::memcmp(allocatedBlocks, allocatedBlocks+1, sizeof(bool)*(BlockSize - 1)));
        }

        T* try_allocate() {
            for(unsigned short i=0; i < BlockSize; i++)
                // This relies on && short circuiting properties. see ISO C++14 ($5.14.1)
                // if a block isn't marked allocated, it marks it and returns the block
                if(!(allocatedBlocks[i]) && (allocatedBlocks[i] = true))
                    return data + i;
            return nullptr;
        }
        inline void deallocate(T* ptr){
            allocatedBlocks[ptr - data] = false;
        }

        bool contains(T* ptr) const {
            // see ... http://stackoverflow.com/a/3692961/1621391
            assert( (reinterpret_cast<std::uintptr_t>(ptr) % alignof(T) == 0) &&
                    " ptr is not suitably aligned!" );
            return (data <= ptr && ptr < (data + BlockSize) ) ;
        }
    };

private:

    Container freeblocks;
    std::deque<MemoryBlock> usedblocks;

};


#endif // MEMORYALLOCATOR_HPP

