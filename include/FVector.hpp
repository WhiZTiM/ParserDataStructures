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

#ifndef FVECTOR_H
#define FVECTOR_H

#include "Config.hpp"
#include <utility>
#include <type_traits>
#include <initializer_list>
using SizeType = uint32_t;

template<typename T>
class FVector{

public:

    using value_type = T;

    struct reserve_tag_t{};
    static reserve_tag_t reserve_tag;

    FVector(){}

    FVector(SizeType sz){
        resize(sz);
    }

    FVector(SizeType sz, reserve_tag_t){
        reserve(sz);
    }

    FVector(std::initializer_list<T> ls){
        emplace_back(ls);
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
        SFAllocator<T>::deallocate(m_data);
    }

    inline SizeType FORCE_INLINE size() const {
        return m_size;
    }

    inline SizeType FORCE_INLINE capacity() const {
        return m_capacity;
    }

    inline FORCE_INLINE T& operator [] (SizeType idx){
        return m_data[idx];
    }

    inline FORCE_INLINE T const& operator [] (SizeType idx) const{
        return m_data[idx];
    }

    void push_back(const T& val){
        emplace_back(T(val));
    }

    void push_back(T&& val){
        emplace_back(std::move(val));
    }

    T& back(){ return m_data[m_size-1]; }
    const T& back() const { return m_data[m_size-1]; }

    void pop_back() {
        call_destructor(m_data[m_size - 1]);
        --m_size;
    }

    template<typename... Args>
    void emplace_back(Args&&... arg){
        if(m_size >= m_capacity)
            //reserve((m_size+1) * 2);
            grow_capacity();
        new(m_data+m_size) T(std::forward<T>(arg)...);
        m_size += 1;
    }

    void emplace_back(std::initializer_list<T> ls){
        if(ls.size() + m_size >= m_capacity){
            if(ls.size() + m_size >= (m_capacity+1)*2)
                reserve(ls.size() + m_size);
            else
                grow_capacity();
        }
        unsigned idx = 0;
        for(auto it = std::begin(ls); it != std::end(ls); ++it)
            new(m_data + m_size + idx++) T(std::move(*it));
        m_size += idx;
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
            SFAllocator<T>::deallocate(m_data);
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

    void inline FORCE_INLINE grow_capacity() { reserve((m_capacity+1) * 2); }

    struct detail {
        template<bool isConst>
        class iterator : public std::iterator<std::random_access_iterator_tag, T>
        {
            template<typename U>
            using Qualified = std::conditional_t<isConst, std::add_const_t<U>, U>;
            //using difference_type = typename std::iterator<std::bidirectional_iterator_tag, T>::difference_type;

            explicit iterator(T* data) : ptr(data){}
        public:
            iterator() = default;
            iterator(const iterator&) = default;
            iterator& operator = (const iterator&) = default;

            Qualified<T>* operator -> () const { return ptr; }
            Qualified<T>& operator * () const { return *ptr; }
            Qualified<iterator>& operator ++ () const { ++ptr; return *const_cast<iterator*>(this); }
            Qualified<iterator> operator ++ (int) const { iterator t(*this); ++ptr; return t; }
            Qualified<iterator>& operator -- () const { --ptr; return *const_cast<iterator*>(this); }
            Qualified<iterator> operator -- (int) const { iterator t(*this); --ptr; return t; }
            Qualified<iterator>& operator += (SizeType idx) const { ptr +=idx; return *const_cast<iterator*>(this); }
            Qualified<iterator>& operator -= (SizeType idx) const { ptr -=idx; return *const_cast<iterator*>(this); }
            Qualified<iterator> operator + (SizeType idx) const { return iterator(ptr + idx); }
            Qualified<iterator> operator - (SizeType idx) const { return iterator(ptr - idx); }
            Qualified<T>& operator [] (std::ptrdiff_t idx) const { return *(ptr + idx); }
            std::ptrdiff_t operator - (const iterator& other) const { return (ptr - other.ptr); }

            friend bool operator == (const iterator& lhs, const iterator& rhs){ return lhs.ptr == rhs.ptr; }
            friend bool operator != (const iterator& lhs, const iterator& rhs){ return!(lhs.ptr == rhs.ptr); }
            friend bool operator <  (const iterator& lhs, const iterator& rhs){ return (rhs.ptr - lhs.ptr) > 0; }
        private:
            friend class FVector<T>;
            mutable T* ptr = nullptr;
        };
    };

public:
    using iterator = typename detail::template iterator<false>;
    using const_iterator = typename detail::template iterator<true>;
    //using const_iterator = detail::iterator<true>;

    iterator begin() {return iterator(m_data); }
    const_iterator begin() const {return const_iterator(m_data); }
    const_iterator cbegin() const {return const_iterator(m_data); }

    iterator end() {return iterator(m_data+m_size); }
    const_iterator end() const {return const_iterator(m_data+m_size); }
    const_iterator cend() const {return const_iterator(m_data+m_size); }

};

template<typename T>
typename FVector<T>::reserve_tag_t FVector<T>::reserve_tag = typename FVector<T>::reserve_tag_t{};

#endif // FVECTOR_H
