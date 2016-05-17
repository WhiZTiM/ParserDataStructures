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
#include "Config.hpp"
#include <type_traits>
#include <forward_list>

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
    T* construct(Args&&... args){
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
                auto iter_e = std::prev(freeblocks.end());
                std::iter_swap(iter1, iter_e);
                freeblocks.pop_back();
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
            assert((this != &other) && " Self assignment isn't permitted!!!!" );
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

        void swap(MemoryBlock& other){
            using std::swap;
            swap(data, other.data);
            swap(allocatedBlocks, other.allocatedBlocks);
        }

        friend void swap(MemoryBlock& lhs, MemoryBlock& rhs){
            lhs.swap(rhs);
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


//! TODO: Add alignment criteria


template<typename T, SizeType BlockSize = 4096>
class ElasticPoolAllocator{

    template<typename U> inline
    std::enable_if_t<std::is_class<U>::value, void>
    FORCE_INLINE call_destructor(U* t){
        t->~U();
    }

    template<typename U> inline
    std::enable_if_t<!std::is_class<U>::value, void>
    FORCE_INLINE call_destructor(U*){}

    class MemoryBlock;

public:

    ElasticPoolAllocator(){
        memblock = new MemoryBlock();
    }

    ~ElasticPoolAllocator(){
        if(memblock->all_memory_has_been_deallocated())
            delete memblock;
    }

    template<typename... Args>
    T* construct(Args&&... args){
        T* t = allocate();
        new(t) T(std::forward<Args>(args)...);
        return t;
    }

    void destruct_and_deallocate(T* t){
        call_destructor(t);
        deallocate(t);
    }

    T* allocate(){
        auto mem = memblock->try_allocate();
        if(mem == nullptr){
            memblock = new MemoryBlock();   //leaks the old memblock (No worries, it will delete itself)
            return memblock->try_allocate();
        }
        return mem;
    }

    void deallocate(T* ptr){
        auto blk = MemoryBlock::deallocate(ptr);
        if(blk->all_memory_has_been_deallocated() && blk->deleteSelf() && blk == memblock )
            memblock = new MemoryBlock();
    }

private:

    class MemoryBlock{
        char* data;
        char* currentIndex;
        int allocationCount = 0;

        static constexpr SizeType PSize = sizeof(char*);
        static constexpr SizeType TSize = std::conditional_t<(sizeof(T) < alignof(T*)),
                                std::integral_constant<int,alignof(T*)>,
                                std::integral_constant<int,sizeof(T)>>::value;

        static constexpr auto dataEnd = (TSize*BlockSize) + (sizeof(void*) * BlockSize);


    public:

        MemoryBlock() :
            data(static_cast<char*>(::operator new((TSize*BlockSize) + (sizeof(void*) * BlockSize))))
        {
            currentIndex = data;
            //std::cout << "Print nullptr: " << (void*)nullptr << " and 0:" << (void*)0 << std::endl;
            //std::cout << "Sizeof(T): " << sizeof(T) << " bytes ...Aligned size: " << TSize << " bytes" << std::endl;
            //std::cout << "Sizeof(char*)==Sizeof(this)==Sizeof(T*)== " << PSize << " bytes" << std::endl;
            //std::cout << "Allocated data @: " << (void*)data << " of size: " << (TSize*BlockSize) + (sizeof(void*) * BlockSize) << std::endl;

            //std::cout << std::endl << std::endl;

            static_assert(sizeof(char*) == sizeof(this), "");
            static_assert(sizeof(this) == PSize, "");
        }

        T* try_allocate(){
            if(currentIndex == (data + dataEnd)){
                return nullptr;
            }
            auto sz = sizeof(this);
            auto this_ = this;
            std::memcpy(reinterpret_cast<MemoryBlock*>(currentIndex), &this_, sz);

            //std::cout << "Copied " << sz << "bytes of (this):" << this << " to data @" << (void*)currentIndex <<std::endl;
            //std::cout << "CurrentIndex: " << (void*)currentIndex << std::endl;

            char* rtn = currentIndex + PSize;
            currentIndex += PSize + TSize;

            //std::cout << "Increased Index by " << PSize + TSize << " bytes" << std::endl;
            //std::cout << "Returned memory address: " << reinterpret_cast<T*>(rtn) << std::endl << std::endl;

            ++allocationCount;
            return reinterpret_cast<T*>(rtn);
        }

        bool all_memory_has_been_deallocated() const { return allocationCount == 0; }
        bool deleteSelf(){ delete this; return true; }

        ~MemoryBlock(){
            ::operator delete(data);
        }

        static MemoryBlock* FORCE_INLINE deallocate(T* ptr){
            MemoryBlock* blk = *reinterpret_cast<MemoryBlock**>(reinterpret_cast<char*>(ptr) - PSize);

            //std::cout << "Was given: " << ptr << std::endl;
            //std::cout << "Resolved MemoryBlock at: " << blk << std::endl;

            --blk->allocationCount;
                //std::cout << "DELETED MEMORY: " << blk << std::endl;
            //std::cout << "currentIndex: " << (void*)blk->currentIndex << std::endl;
            return blk;
        }

    };

    MemoryBlock* memblock;
};



#endif // MEMORYALLOCATOR_HPP

