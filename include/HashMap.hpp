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


#ifndef HASHMAP_H
#define HASHMAP_H

#include <memory>
#include <vector>
#include <cstring>
#include <forward_list>
#include "Config.hpp"
#include <iostream>
#include <String.hpp>

using namespace std;

template<std::size_t max = 1000000,
         typename Type = typename std::conditional<
             (max <= std::numeric_limits<std::size_t>::max()), std::size_t, unsigned long
             >::type
         >
class EratosthenesSieve{
public:
    static_assert(max > 2, "Sieve size must be greater than 2");

    using value_type = Type;

    inline std::vector<Type> sieve() const {

        std::vector<Type> rtn;
        rtn.reserve(max);
        bool flag[max];
        //! std::fill will use memset
        std::fill(std::begin(flag) + 2, std::end(flag), false);
        std::fill(std::begin(flag), std::begin(flag) + 2, true);
        for(std::size_t index = 2; index < max; index++)
            if(not flag[index])
                for(std::size_t i = 2*index; i < max; i += index)
                    flag[i] = true;

        for(std::size_t idx = 0; idx < max; idx++)
            if(not flag[idx]){
                rtn.emplace_back(idx);
            }

        return rtn;
    }
};

template<typename T>
inline SizeType FORCE_INLINE hash_it(const T& t){
    return std::hash<T>()(t);
}

//Courtesy of http://programmers.stackexchange.com/a/49566/220592
template<>
inline SizeType FORCE_INLINE hash_it<FString>(const FString& t){
        unsigned long hash = 5381;
        int c;
        const unsigned char *str = reinterpret_cast<const unsigned char*>(t.c_str());
        while ((c = *str++))
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
        return hash;
}

template<typename Key, typename Value>
class HashMap
{


    struct HashNode{
        std::pair<const Key, Value> data;
        HashNode* next;
    };

    template<bool isConst>
    class Iterator : public std::iterator<
            std::forward_iterator_tag,
            std::pair<const Key, Value>
            >
    {

        template<typename U>
        using Qualified = std::conditional_t<isConst, std::add_const_t<U>, U>;
        using value_type = std::pair<const Key, Value>;

        Iterator(HashMap<Key, Value>* hmp) : hashMap(hmp) {
            currentNode = go_to_next(true);
        }

        Iterator(HashMap<Key, Value> *hmp, HashNode* nd, SizeType index) :
            hashMap(hmp), currentNode(nd), idx(index) {
            //
        }

    public:
        Iterator(){}
        ~Iterator(){}
        Iterator(const Iterator& other) = default;
        Iterator& operator = (const Iterator& other) = default;
        operator Iterator<true> () { return Iterator<true>{hashMap, currentNode, idx}; }

        Qualified<value_type>& operator* () const {
            return currentNode->data;
        }

        Qualified<Iterator>& operator ++ () const {
            go_to_next();
            return *const_cast<Iterator*>(this);
        }
        Qualified<Iterator> operator ++ (int) const {
            Iterator tmp(*this);
            go_to_next();
            return tmp;
        }

        friend bool operator == (const Iterator& lhs, const Iterator& rhs){
            return lhs.currentNode == rhs.currentNode;
        }
        friend bool operator != (const Iterator& lhs, const Iterator& rhs){
            return ! (lhs.currentNode == rhs.currentNode);
        }

    private:
        friend class HashMap<Key, Value>;
        HashMap<Key, Value>* hashMap = nullptr;
        mutable HashNode* currentNode = nullptr;
        mutable SizeType idx = 0;

        HashNode* go_to_next(bool consider_currentNode = true) const {
            if((consider_currentNode && !currentNode) || !consider_currentNode)
                for(; idx < hashMap->m_bucketSize; idx++){
                    if(hashMap->m_buckets[idx]){
                        currentNode = hashMap->m_buckets[idx++];
                        return currentNode;
                    }
                }
            else
                currentNode = currentNode->next ? currentNode->next : go_to_next(false);
            return nullptr;

        }
    };

public:
    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;

    iterator begin()                { return iterator(this); }
    const_iterator begin() const    { return const_iterator(const_cast<HashMap*>(this)); }
    const_iterator cbegin() const   { return const_iterator(const_cast<HashMap*>(this)); }

    iterator end()                  { return iterator(); }
    const_iterator end() const      { return const_iterator(); }
    const_iterator cend() const     { return const_iterator(); }

    public:
        using value_type = std::pair<const Key&, Value&>;

        HashMap() {  /*******/  }
        ~HashMap(){  destroy(); }

        HashMap(HashMap&& other) noexcept {
            move_from(std::move(other));
        }

        HashMap(const HashMap& other) {
            copy_from(other);
        }

        HashMap& operator=(HashMap&& other) noexcept{
            if(this == &other) return *this;
            destroy();
            move_from(std::move(other));
        }

        HashMap& operator=(const HashMap& other){
            if(this == &other) return *this;
            destroy();
            copy_from(other);
        }

        inline SizeType FORCE_INLINE size() const {
            return m_nodeSize();
        }

        void insert(std::pair<const Key, Value>&& kv){
            static std::vector<SizeType> prVec = EratosthenesSieve<1000000, SizeType>().sieve();

            if(m_nodeSize >= m_bucketSize * 1.5)    //load factor of  1 / 1.5  =  0.6666667
                reserve(prVec[m_bucketSize+7]);
            cout << "m_buckets is now: " << m_buckets << endl;
            imbue_data(kv.first, kv.second, m_buckets, m_bucketSize);
        }

        Value& operator [] (const Key& ky){
            return (*getNode(ky)).second;
        }

        const Value& operator [] (const Key& ky) const {
            return (*getNode(ky)).second;
        }

        iterator find(const Key& ky) {
            return getNode(ky);
        }

        const_iterator find(const Key& ky) const {
            return const_cast<HashMap*>(this)->getNode(ky);
        }

        void erase(const iterator& iter){
            erase(iter.currentNode->data.first);
        }

        void erase(const Key& ky){
            auto index = hash(ky, m_bucketSize);
            auto& bucket = m_buckets[index];
            auto left = bucket;

            if(bucket && bucket->data.first == ky){
                auto nxt = bucket->next;
                delete bucket;
                bucket = nxt;
            }

            while(left->next){
                auto nxt = left->next->next;
                if(left->next->data.first == ky)
                    delete nxt;
                left->next = nxt;
            }
        }

        inline void reserve(SizeType sz){
            if(sz > m_bucketSize){
                void* addr = SFAllocator<HashNode**>::allocate(sz);
                HashNode** data = static_cast<HashNode**>(addr);
                HashNode* ad2 = static_cast<HashNode*>(addr);

                //cout << "Address of void* addr: " << addr << endl;
                //cout << "Address of HashNode** data: " << data << endl;
                //cout << "Address of HashNode* ad2: " << ad2 << endl;

                (void)ad2;
                for(SizeType i = 0; i < sz; i++)
                    data[i] = nullptr;
                SizeType counter = 0;
                for(SizeType i = 0; i < m_bucketSize; i++){
                    if(m_buckets[i]){
                        imbue_data(m_buckets[i]->data.first, m_buckets[i]->data.second, data, sz, counter);
                        for(auto link = m_buckets[i]->next; link; link = link->next)
                            imbue_data(m_buckets[i]->data.first, m_buckets[i]->data.second, data, counter);
                        delete m_buckets[i];
                    }
                }
                SFAllocator<HashNode*>::deallocate(m_buckets);
                m_nodeSize = counter;
                m_bucketSize = sz;
                m_buckets = data;
            }
        }

        inline SizeType FORCE_INLINE hash(const Key& ky, SizeType sz) const {
            cout << "$$$$$$$$$$$$$$KEY: "<< ky << " $$$HASH: " << hash_it(ky) << " % " << sz << " = " << hash_it(ky) % sz << endl;
            return hash_it(ky) % sz;
        }

    private:

        HashNode** m_buckets = nullptr;
        SizeType m_bucketSize = 0;
        SizeType m_nodeSize = 0;

        inline void FORCE_INLINE move_from(HashMap&& other){
            m_buckets = other.m_buckets;
            m_bucketSize = other.m_bucketSize;
            m_nodeSize = other.m_nodeSize;
            other.m_bucketSize = other.m_nodeSize = 0;
            other.m_buckets = nullptr;
        }

        inline void FORCE_INLINE copy_from(const HashMap& other){
            reserve(other.m_bucketSize);
            throw std::runtime_error("Not yet implemented");
        }

        inline void destroy() noexcept {
            cout << "Destructor: m_buckets:" << m_buckets << endl;
            for(SizeType i=0; i < m_bucketSize; i++){
                cout << "Destructor: m_buckets[" << i << "] : " << m_buckets[i] << endl;
                if(m_buckets[i]){
                    for(auto node = m_buckets[i]->next; node != nullptr;){
                        auto currentNode = node;
                        node = node->next;
                        delete currentNode;
                        --m_nodeSize;
                    }
                    delete m_buckets[i];
                    --m_nodeSize;
                }
            }
            SFAllocator<HashNode**>::deallocate(m_buckets);
        }

        inline HashNode* imbue_data(const Key& ky, Value& val, HashNode** mem, SizeType memSize){
            return imbue_data(ky, val, mem, memSize, m_nodeSize);
        }

        inline HashNode* imbue_data(const Key& ky, Value& val, HashNode** mem, SizeType memSize, SizeType& counter){
            auto index = hash(ky, memSize);
            HashNode*& node = mem[index];
            //cout << "Node is " << node << "\t\tHashed index to be: " << index << endl;
            if(node){
                HashNode* link = node;
                if(node->data.first == ky){
                    //cout << " Didn't add " << ky << " : " << val << " primary node at pos: " << hash(ky, memSize) << endl;
                    return node;
                }
                else{
                    for(link = node; link->next; link = link->next)
                        if(link->data.first == ky)
                            return link;
                }
                //cout << "\t\tAND LINK.Key is: " << link->key << endl;
                link->next = new HashNode{ {ky, std::move(val)}, nullptr };
                ++counter;
                cout << "added " << ky << " : " << val << " secondary node at pos: " << hash(ky, memSize) << endl;
                return link->next;
            }
            node = new HashNode{ {ky, std::move(val)}, nullptr };
            cout << "NODE ADDED KEY: key=" << ky << "     node->key=" << node->data.first << endl;
            //delete node;
            //node = new HashNode{ ky, std::move(val), nullptr };
            ++counter;
            //cout << " added " << ky << " : " << val << " primary node at pos: " << hash(ky, memSize) << "  addr: " << node << endl;
            return node;
        }

        inline iterator FORCE_INLINE getNode(const Key& ky) {
            SizeType idx = hash(ky, m_bucketSize);
            HashNode* node = m_buckets[idx];
            if(node){
                HashNode* link = node;
                if(node->data.first == ky){
                    return iterator(this, node, idx);
                }
                else{
                    for(link = node->next; link; link = link->next)
                        if(link->data.first == ky)
                            return iterator(this, link, idx);
                }
            }
            return iterator{};
        }

};

#endif // HASHMAP_H
