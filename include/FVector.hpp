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
#include <iterator>
using SizeType = uint32_t;

template<typename T>
class FVector{

    struct detail {

        template<bool IsConst>
        class iterator
        {
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = std::conditional_t<IsConst, std::add_const_t<T>, T>;
            using pointer = value_type*;
            using reference = value_type&;
            using iterator_category = std::random_access_iterator_tag;

        private:
            explicit iterator(T* data) : ptr(data){}
        public:
            iterator() = default;
            iterator(const iterator&) = default;
            iterator& operator = (const iterator&) = default;

            operator iterator<true> () const { return iterator<true>(ptr); }

            pointer operator -> () const { return ptr; }
            reference operator * () const { return *ptr; }
            iterator& operator ++ () { ++ptr; return *const_cast<iterator*>(this); }
            iterator operator ++ (int) { iterator t(*this); ++ptr; return t; }
            iterator& operator -- () { --ptr; return *const_cast<iterator*>(this); }
            iterator operator -- (int) { iterator t(*this); --ptr; return t; }
            iterator& operator += (int idx) { ptr +=idx; return *const_cast<iterator*>(this); }
            iterator& operator -= (int idx) { ptr -=idx; return *const_cast<iterator*>(this); }
            iterator operator + (int idx) const { return iterator(ptr + idx); }
            iterator operator - (int idx) const { return iterator(ptr - idx); }
            reference operator [] (std::ptrdiff_t idx) const { return *(ptr + idx); }
            std::ptrdiff_t operator - (const iterator& other) const { return (ptr - other.ptr); }

            friend bool operator == (const iterator& lhs, const iterator& rhs){ return lhs.ptr == rhs.ptr; }
            friend bool operator != (const iterator& lhs, const iterator& rhs){ return!(lhs.ptr == rhs.ptr); }
            friend bool operator <  (const iterator& lhs, const iterator& rhs){ return (rhs.ptr - lhs.ptr) > 0; }
            friend bool operator >  (const iterator& lhs, const iterator& rhs){ return (lhs.ptr - rhs.ptr) > 0; }
            friend bool operator <=  (const iterator& lhs, const iterator& rhs){ return (rhs.ptr - lhs.ptr) >= 0; }
            friend bool operator >=  (const iterator& lhs, const iterator& rhs){ return (lhs.ptr - rhs.ptr) >= 0; }
        private:
            friend class FVector<T>;
            T* ptr = nullptr;
        };

    };

public:
    //Forward Iterators
    using iterator = typename detail::template iterator<false>;
    using const_iterator = typename detail::template iterator<true>;

    iterator begin() {return iterator(m_data); }
    const_iterator begin() const {return const_iterator(m_data); }
    const_iterator cbegin() const {return const_iterator(m_data); }

    iterator end() {return iterator(m_data+m_size); }
    const_iterator end() const {return const_iterator(m_data+m_size); }
    const_iterator cend() const {return const_iterator(m_data+m_size); }

    //Reverse Iterators
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    reverse_iterator rbegin() {return reverse_iterator(iterator(m_data + m_size)); }
    const_reverse_iterator rbegin() const {return const_reverse_iterator(iterator(m_data + m_size)); }
    const_reverse_iterator crbegin() const {return const_reverse_iterator(iterator(m_data + m_size)); }

    reverse_iterator rend() {return reverse_iterator(iterator(m_data)); }
    const_reverse_iterator rend() const {return const_reverse_iterator(iterator(m_data)); }
    const_reverse_iterator crend() const {return const_reverse_iterator(iterator(m_data)); }

public:

    using value_type = T;
    using size_type = SizeType;
    using pointer = value_type*;
    using reference = value_type&;
    using const_pointer = const value_type*;
    using const_reference = const value_type&;
    using difference_type = std::ptrdiff_t;

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

    inline bool FORCE_INLINE empty() const {
        return m_size == 0;
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

    inline FORCE_INLINE T& at(SizeType idx) {
        if(!(idx < m_size))
            throw std::out_of_range("Invalid Range given");
        return m_data[idx];
    }

    inline FORCE_INLINE T const& at(SizeType idx) const{
        return const_cast<FVector*>(this)->at(idx);
    }

    void push_back(const T& val){
        emplace_back(T(val));
    }

    void push_back(T&& val){
        emplace_back(std::move(val));
    }

    void pop_back() {
        call_destructor(m_data[m_size - 1]);
        --m_size;
    }

    T& back(){ return m_data[m_size-1]; }
    const T& back() const { return m_data[m_size-1]; }

    T& front(){ return m_data[0]; }
    const T& front() const { return m_data[0]; }

    void clear() noexcept {
        for(SizeType i=0; i<m_size; i++)
            call_destructor(m_data[i]);
        m_size = 0;
    }

    iterator erase(const_iterator pos){
        return erase(pos, pos + 1);
    }

    iterator erase(const_iterator first, const_iterator last){
        int S = first - cbegin();
        int L = last - cbegin();
        int diff = L - S;
        if(diff <= 0 || int(m_size) - diff <= 0)
            return end();

        for(int i = S; i < L; i++){
            call_destructor(m_data[i]);
        }
        for(int i = S; unsigned(i + diff) < m_size; i++){
            new (m_data+i) T(std::move(m_data[i + diff]));
            call_destructor(m_data[i + diff]);
        }
        m_size -= diff;
        auto kpx = m_data + (L - 1);
        return iterator(kpx);
    }

    template<typename... Args>
    void emplace_back(Args&&... arg){
        if(m_size >= m_capacity)
            grow_capacity();    //should be same as calling reserve((m_size+1) * 2);
        new(m_data+m_size) T(std::forward<Args&&>(arg)...);
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

    void swap(FVector& other){
        using std::swap;
        swap(m_size, other.m_size);
        swap(m_data, other.m_data);
        swap(m_capacity, other.m_capacity);
    }

    friend void swap(FVector& lhs, FVector& rhs){   //for ADL
        lhs.swap(rhs);
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

    // Unlike C++'s STL, this is a binding request
    void shrink_to_fit(){
        if(m_size < m_capacity){
            T* data = static_cast<T*>(SFAllocator<T>::allocate(m_size));
            for(SizeType i=0; i < m_size; i++){
                new(data+i) T(std::move(m_data[i]));       //! TODO: move if only noexcept;
                call_destructor(m_data[i]);
            }
            SFAllocator<T>::deallocate(m_data);
            m_capacity = m_size;
            m_data = data;
        }
    }

    void assign(size_type count, const T& value){
        reserve(count);
        size_type i = 0, sc = std::min(count, m_size);
        for(; i < sc; i++)
            operator [](i) = value;
        for(; i < count; i++)
            new(m_data + i) T(value);
        m_size = count;
    }

    void assign(std::initializer_list<T> ilist){
        reserve(ilist.size());
        size_type i = 0, sc = std::min(ilist.size(), std::size_t(m_size));
        auto it = std::begin(ilist);
        for(; it != std::end(ilist); ++it)
            operator [](i++) = *it;
        for(; it != std::end(ilist); ++it)
            new(m_data + i++) T(*it);
        m_size = ilist.size();
    }

    template< class InputIt >
    void assign( InputIt first, InputIt last ){
        size_type i = 0;
        for(; (first != last) && (i < m_size); ++first, i++)
            operator [](i) = *first;
        for(; first != last; ++first, i++)
            emplace_back(*first);
        m_size = i;
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
};

template<typename T>
typename FVector<T>::reserve_tag_t FVector<T>::reserve_tag = typename FVector<T>::reserve_tag_t{};


#endif // FVECTOR_H
