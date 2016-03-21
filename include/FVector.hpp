#ifndef FVECTOR_H
#define FVECTOR_H

#include "Config.hpp"
#include <utility>
#include <initializer_list>
using SizeType = uint32_t;

template<typename T>
class FVector{

public:
    struct reserve_tag_t{};
    static reserve_tag_t reserve_tag;

    FVector(){}

    FVector(SizeType sz){
        reserve(sz);
    }

    FVector(SizeType sz, reserve_tag_t){
        resize(sz);
    }

    FVector(std::initializer_list<T> ls){
        for(auto& x : ls)
            push_back(x);
    }

    FVector(FVector&& other) noexcept {
        move_from(std::move(other));
    }

    FVector(const FVector& other){
        copy_from(other);
    }

    FVector& operator = (FVector&& other) noexcept {
        if(this == &other) return *this;
        move_from(std::move(other));
    }

    FVector& operator = (const FVector& other) noexcept {
        if(this == &other) return *this;
        copy_from(other);
    }

    ~FVector() noexcept {
        SFAllocator<T>::dellocate(m_data);
    }

    inline SizeType FORCE_INLINE size() const {
        return m_size;
    }

    inline SizeType capacity() const {
        return m_capacity;
    }

    inline T& FORCE_INLINE operator [] (SizeType idx){
        return m_data[idx];
    }

    inline T const& FORCE_INLINE operator [] (SizeType idx) const{
        return m_data[idx];
    }

    void push_back(const T& val){
        emplace_back(T(val));
    }

    void push_back(T&& val){
        emplace_back(std::move(val));
    }

    template<typename... Args>
    void emplace_back(Args&&... arg){
        if(m_size >= m_capacity)
            reserve((m_size+1) * 2);
        new(m_data+m_size) T(std::forward<T>(arg)...);
        m_size += 1;
    }

    template<typename... Arg>
    void resize(SizeType sz, Arg&&... arg){
        reserve(sz);
        if(sz > m_size)
            for(SizeType i=m_size; i<sz; i++)
                new(m_data+i) T(std::forward(arg)...);
        else
            for(SizeType i=sz; i<m_size; i++)
                call_destructor(m_data[i]);
        m_size = sz;
    }

    void reserve(SizeType sz){
        if(sz > m_capacity){
            T* data = static_cast<T*>(SFAllocator<T>::allocate(sz));
            for(SizeType i=0; i < m_size; i++){
                new(data+i) T(std::move(m_data[i]));       //! TODO: move if only noexcept;
                call_destructor(m_data[i]);
            }
            SFAllocator<T>::dellocate(m_data);
            m_capacity = sz;
            m_data = data;
        }
    }

    void clear() noexcept {
        for(SizeType i=0; i<m_size; i++)
            call_destructor(m_data[i]);
        m_size = 0;
    }

    void swap(FVector& other){
        using std::swap;
        swap(m_size, other.m_size);
        swap(m_data, other.m_data);
        swap(m_capacity, other.m_capacity);
    }

    inline void FORCE_INLINE move_from(FVector&& other){
        clear();
        swap(other);
    }

    inline void FORCE_INLINE copy_from(const FVector& other){
        clear();
        reserve(other.m_size);
        for(SizeType i=0; i < m_capacity; i++)
            new(m_data+i) T(*(other.m_data[i]));
    }

private:
    T* m_data = nullptr;
    SizeType m_capacity = 0;
    SizeType m_size = 0;

    template<typename U> inline
    std::enable_if_t<std::is_class<U>::value, void>
    FORCE_INLINE call_destructor(U& t){
        t.~U();
    }

    template<typename U> inline
    std::enable_if_t<!std::is_class<U>::value, void>
    FORCE_INLINE call_destructor(U&){}

};

template<typename T>
typename FVector<T>::reserve_tag_t FVector<T>::reserve_tag = typename FVector<T>::reserve_tag_t{};

#endif // FVECTOR_H
