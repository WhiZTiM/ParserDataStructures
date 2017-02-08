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

#ifndef STRING_H
#define STRING_H
#include "Config.hpp"
#include <cstring>
#include <utility>
#include <ostream>
#include <istream>
#include <cassert>
#include <type_traits>

template<typename Char>
class Basic_fstring
{
    public:

    using value_type = Char;
    using size_type = SizeType;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;
    using const_pointer = const value_type*;
    using const_reference = const value_type&;

    //Itertors are defined at the bottom

        //Defined as  static_cast<size_type>(-1);
        static const size_type npos;

        //! kSS is the constant for Short String Optimisation
        //! To prevent wastage from padding, it should be
        //! a multiple of 8 bytes on 64 bit systems, and worse
        //! 4 bytes on 32 bit systems
        static constexpr int kSS = 8;

        //! Constructs an empty string, very fast
        Basic_fstring(){ get_pointer()[0] = '\0'; }

        //! Constructs a String from a string literal, faster than any STL implementation
        template<SizeType N>
        Basic_fstring(const Char (&data)[N]){
            copy_construct_from(data, N);
        }

        //! Constructs a String from a pointer to an array of character string.
        //! NOTE: the string must be NULL terminated, else, the behavior is Undefined
        //! This constructor only part
        template<typename T, std::enable_if_t<std::is_same<T, const Char*>::value>* = nullptr>
        explicit Basic_fstring(const T ch){
            copy_construct_from(ch, strlen(ch)+1);
        }

        Basic_fstring(const std::basic_string<Char>& str){
            copy_construct_from(str.c_str(), str.size()+1);
        }

        FORCE_INLINE ~Basic_fstring() { destroy(); }

        Basic_fstring(const Basic_fstring& other){
            destroy();
            copy_from(other);
        }

        Basic_fstring(Basic_fstring&& other) noexcept{
            move_from(std::move(other));
        }

        FORCE_INLINE Basic_fstring& operator = (Basic_fstring&& other) noexcept {
            if(this == &other)
                return *this;
            destroy();
            move_from(std::move(other));
            return *this;
        }

        FORCE_INLINE Basic_fstring& operator = (const Basic_fstring& other){
            if(this == &other)
                return *this;
            destroy();
            copy_from(other);
            return *this;
        }

        inline FORCE_INLINE Char* data () {
            return get_pointer();
        }

        inline FORCE_INLINE const Char* data () const {
            return const_cast<Basic_fstring*>(this)->data();
        }

        inline FORCE_INLINE const Char* c_str () const {
            return data();
        }

        inline FORCE_INLINE std::basic_string<Char> to_string() const {
            return empty() ? std::basic_string<Char>() : std::basic_string<Char>(data());
        }

        inline FORCE_INLINE Char& operator [] (SizeType idx) {
            return get_pointer()[idx];
        }

        inline FORCE_INLINE Char const& operator [] (SizeType idx) const {
            return const_cast<Basic_fstring&>(*this).operator[](idx);
        }

        inline FORCE_INLINE SizeType size() const {
            return m_size;
        }

        bool FORCE_INLINE empty() const {
            return m_size == 0;
        }

        void FORCE_INLINE clear() noexcept {
            destroy();
        }

        void swap(Basic_fstring& other){
            std::swap(m_data, other.m_data);
            std::swap(m_size, other.m_size);
        }

        Basic_fstring substr(size_type pos, size_type count = npos) const {
            const auto string_size = this->size();
            assert(pos <= string_size && "starting index must be less than the size of this string!");
            Basic_fstring temp;
            count = (count >= string_size || overflows_by_addition(pos, count) || pos+count > string_size) ?
                        string_size - pos : count;
            temp.copy_construct_from(get_pointer()+pos, count+1);

            // explicitly terminate the string since we do not know whether the
            // resulting substring is nulll terminated
            temp[count] = '\0';
            return temp;
        }

        [[deprecated]]
        Basic_fstring portion(size_type start, size_type stop = npos) const {
            const auto string_size = this->size();
            assert(start <= stop && "interval must be positive!");
            assert(stop <= string_size && "end index must be less than the size of this string!");
            assert(start <= string_size && "starting index must be less than the size of this string!");
            Basic_fstring temp;
            stop = (stop == npos) ? stop - start : stop;
            temp.copy_construct_from(get_pointer()+start, stop+1);

            // explicitly terminate the string since we do not know whether the
            // resulting substring is nulll terminated
            temp[stop] = '\0';
            return temp;
        }

        size_type find(const Basic_fstring& str, size_type pos = 0) const {
            for(unsigned i = pos; i < size(); i++){
                unsigned j = 0;
                for(; j < str.size() && (i+j) < size(); j++)
                    if(operator [](i+j) != str[j])
                        break;
                if(j == str.size())
                    return i;
            }
            return npos;
        }

        FORCE_INLINE size_type find(Char ch, size_type pos = 0) const {
            for(unsigned i = pos; i < size(); i++)
                if(operator [](i) == ch)
                    return i;
            return npos;
        }

        size_type rfind(const Basic_fstring& str, size_type pos = npos) const {
            if(size() == 0 || str.size() == 0) return npos;
            pos = pos == npos ? size() : (pos + 1);
            for(unsigned i = pos - 1; ; i--){
                unsigned j = str.size() - 1;
                for(unsigned q = 0; i >= j; j--, q++){
                    if(operator [](i-q) != str[j])
                        break;
                    if(j == 0)
                        return (i - q);
                }
                 if(i == 0)
                     break;
            }
            return npos;
        }

        FORCE_INLINE size_type rfind(Char ch, size_type pos = npos) const {
            if(size() == 0) return npos;
            pos = pos == npos ? size() : (pos + 1);
            for(unsigned i = pos - 1; ; i--){
                if(operator [](i) == ch)
                    return i;
                if(i == 0)
                    break;
            }
            return npos;
        }

        inline static int FORCE_INLINE compare(Basic_fstring const& lhs, Basic_fstring const& rhs) noexcept {
            if(lhs.m_size < kSS){
                if(rhs.m_size < kSS)
                    return std::strncmp(static_cast<const Char*>(lhs.m_data.local),
                                        static_cast<const Char*>(rhs.m_data.local),
                                        lhs.m_size < rhs.m_size ? lhs.m_size : rhs.m_size);
                else
                    return std::strncmp(static_cast<const Char*>(lhs.m_data.local),
                                        rhs.m_data.heap, rhs.m_size);
            }
            else
                if(rhs.m_size < kSS)
                    return std::strncmp(lhs.m_data.heap, static_cast<const Char*>(rhs.m_data.local),
                                        lhs.m_size);
            return std::strncmp(lhs.m_data.heap, rhs.m_data.heap,
                                lhs.m_size < rhs.m_size ? lhs.m_size : rhs.m_size);
        }

        template<Char> friend
        std::basic_istream<Char>& operator >> (std::basic_istream<Char>&, Basic_fstring<Char>&);

        template<Char> friend
        std::basic_ostream<Char>& operator << (std::basic_ostream<Char>&, Basic_fstring<Char> const&);

        template<Char> friend void swap(Basic_fstring&, Basic_fstring&);

        union Data {
            Char local[kSS];
            Char* heap;
        };
    private:

        Data m_data;
        SizeType m_size = 0;

        inline FORCE_INLINE Char* get_pointer() const {
            return m_size < kSS ? const_cast<Char*>(static_cast<const Char*>(m_data.local)) : m_data.heap;
        }

        inline FORCE_INLINE void move_from(Basic_fstring&& other){
            m_data = other.m_data;
            m_size = other.m_size;
            other.m_size = 0;
        }
        inline FORCE_INLINE void copy_from(const Basic_fstring& other){
            if(other.m_size >= kSS){
                m_size = other.m_size;
                m_data.heap = static_cast<Char*>(operator new (sizeof(Char) * (other.m_size+1)));// new Char[other.m_size+1];
                std::memcpy(m_data.heap, other.m_data.heap, other.m_size+1);
            }
            else{
                m_data = other.m_data;
                m_size = other.m_size;
            }
        }

        inline FORCE_INLINE void copy_construct_from(const Char* ch, SizeType sz){
            m_size = sz - 1;
            if(sz < kSS)
                std::memcpy(&m_data.local, ch, sizeof(Char)*sz);
            else{
                m_data.heap = static_cast<Char*>(operator new (sizeof(Char) * sz));
                std::memcpy(m_data.heap, ch, sizeof(Char)*sz);
            }
        }

        void FORCE_INLINE destroy() noexcept {
            if(!(m_size < kSS))
                delete m_data.heap;
            m_size = 0;
        }

        struct detail {

            template<bool isConst>
            class iterator //: public std::iterator<std::random_access_iterator_tag, Char>
            {

            public:
                using difference_type = std::ptrdiff_t;
                using value_type = std::conditional_t<isConst, std::add_const_t<Char>, Char>;
                using pointer = value_type*;
                using reference = value_type&;
                using iterator_category = std::random_access_iterator_tag;

            private:
                iterator(Char* data) : ptr(data){}
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
                friend bool operator >=  (const iterator& lhs, const iterator& rhs){ return (lhs.ptr - rhs.ptr) >= 0; }
                friend bool operator <=  (const iterator& lhs, const iterator& rhs){ return (rhs.ptr - lhs.ptr) >= 0; }
            private:
                friend class Basic_fstring<Char>;
                pointer ptr = nullptr;
            };
        };

    public:
        //Normal Iterators
        using iterator = typename detail::template iterator<false>;
        using const_iterator = typename detail::template iterator<true>;

        iterator begin() {return iterator(get_pointer()); }
        const_iterator begin() const {return const_iterator(get_pointer()); }
        const_iterator cbegin() const {return const_iterator(get_pointer()); }

        iterator end() {return iterator(get_pointer()+m_size); }
        const_iterator end() const {return const_iterator(get_pointer()+m_size); }
        const_iterator cend() const {return const_iterator(get_pointer()+m_size); }

        //Reverse Iterators
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        reverse_iterator rbegin() {return iterator(get_pointer()+m_size); }
        const_reverse_iterator rbegin() const {return const_reverse_iterator(get_pointer()+m_size); }
        const_reverse_iterator crbegin() const {return const_reverse_iterator(get_pointer()+m_size); }

        reverse_iterator rend() {return reverse_iterator(get_pointer()); }
        const_reverse_iterator rend() const {return const_reverse_iterator(get_pointer()); }
        const_reverse_iterator crend() const {return const_reverse_iterator(get_pointer()); }

};

template<typename Char>
const typename Basic_fstring<Char>::size_type Basic_fstring<Char>::npos = static_cast<size_type>(-1);

template<typename Char> inline FORCE_INLINE
bool operator == (const Basic_fstring<Char>& lhs, const Basic_fstring<Char>& rhs){
    return Basic_fstring<Char>::compare(lhs, rhs) == 0;
}

template<typename Char, SizeType N> inline FORCE_INLINE
bool operator == (const Basic_fstring<Char>& lhs, const Char (&rhs)[N]){
    return Basic_fstring<Char>::compare(lhs, rhs) == 0;
}

template<typename Char, SizeType N> inline FORCE_INLINE
bool operator == (const Char (&lhs)[N], const Basic_fstring<Char>& rhs){
    return Basic_fstring<Char>::compare(lhs, rhs) == 0;
}

template<typename Char> inline FORCE_INLINE
bool operator != (Basic_fstring<Char> const& lhs, Basic_fstring<Char> const& rhs){
    return !(lhs == rhs);
}

template<typename Char, SizeType N> inline FORCE_INLINE
bool operator != (const Basic_fstring<Char>& lhs, const Char (&rhs)[N]){
    return !(lhs == rhs);
}

template<typename Char, SizeType N> inline FORCE_INLINE
bool operator != (const Char (&lhs)[N], const Basic_fstring<Char>& rhs){
    return !(lhs == rhs);
}

template<typename Char> inline
bool operator < (Basic_fstring<Char> const& lhs, Basic_fstring<Char> const& rhs){
    return Basic_fstring<Char>::compare(lhs, rhs) < 0;
}

template<typename Char> inline
bool operator <= (Basic_fstring<Char> const& lhs, Basic_fstring<Char> const& rhs){
    return Basic_fstring<Char>::compare(lhs, rhs) <= 0;
}

template<typename Char> inline
bool operator > (Basic_fstring<Char> const& lhs, Basic_fstring<Char> const& rhs){
    return Basic_fstring<Char>::compare(lhs, rhs) > 0;
}

template<typename Char> inline
bool operator >= (Basic_fstring<Char> const& lhs, Basic_fstring<Char> const& rhs){
    return Basic_fstring<Char>::compare(lhs, rhs) >= 0;
}

template<typename Char>
inline std::basic_istream<Char>& operator >> (std::basic_istream<Char>& i, Basic_fstring<Char>& str){
    std::basic_string<Char> sstr; i >> sstr;
    str = sstr;
    return i;
}

template<typename Char>
inline std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& o, Basic_fstring<Char> const& str){
    if(str.size() > 0)
        o << &str[0];
    return o;
}

namespace std {
    template<typename Char>
    inline std::basic_istream<Char>& getline(std::basic_istream<Char>& i, Basic_fstring<Char>& str){
        std::basic_string<Char> sstr; std::getline(i, sstr);
        return str = sstr, i;
    }
}

template<typename Char>
inline void swap(Basic_fstring<Char>& lhs, Basic_fstring<Char>& rhs){
    lhs.swap(rhs);
}

using FString = Basic_fstring<char>;
using FWString = Basic_fstring<wchar_t>;
using F16String = Basic_fstring<char16_t>;
using F32String = Basic_fstring<char32_t>;

#endif // STRING_H
