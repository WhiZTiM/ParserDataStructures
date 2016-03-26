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

#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H

#include <new>
#include <iterator>
#include <type_traits>

#ifdef __GNUC__
#define FORCE_INLINE __attribute__((always_inline))
#elif _MSC_VER
#define FORCE_INLINE __forceinline
#endif // __GNUC__


using SizeType = unsigned int;

template <typename T>
class SFAllocator {
public:
    static inline void* FORCE_INLINE allocate(SizeType sz){
        return operator new(sz * sizeof(T));
    }
    static inline void FORCE_INLINE deallocate(void* m){
        operator delete (m);
    }
};

template <typename T>
inline void nulled_delete(T* ptr){
    delete ptr;
    ptr = nullptr;
}

#endif // PLATFORM_CONFIG_H
