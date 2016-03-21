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
    static inline void FORCE_INLINE dellocate(void* m){
        operator delete (m);
    }
};

#endif // PLATFORM_CONFIG_H
