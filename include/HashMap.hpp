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

template<typename T>
inline SizeType FORCE_INLINE hash_it(const T& t){
    return std::hash<T>()(t);
}

template<>
inline SizeType FORCE_INLINE hash_it<FString>(const FString& t){
    return std::hash<const char*>()(t.c_str());
}

template<typename Key, typename Value>
class HashMap
{
    public:
        using value_type = std::pair<const Key&, Value&>;
        class iterator;
        using const_iterator = const iterator;

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

        void insert(std::pair<Key, Value>&& kv){
            if(m_nodeSize == m_bucketSize)
                reserve((m_bucketSize+1) * 2);
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
            return getNode(ky);
        }

        void erase(const iterator& iter){
            erase(iter.currentNode->key);
        }

        void erase(const Key& ky){
            auto index = hash(ky, m_bucketSize);
            auto& bucket = m_buckets[index];
            auto left = bucket;

            if(bucket && bucket->key == ky){
                auto nxt = bucket->next;
                delete bucket;
                bucket = nxt;
            }

            while(left->next){
                auto nxt = left->next->next;
                if(left->next->key == ky)
                    delete nxt;
                left->next = nxt;
            }
        }

        iterator begin()                { return iterator(this); }
        const_iterator begin() const    { return iterator(this); }
        const_iterator cbegin() const   { return iterator(this); }

        iterator end()                  { return iterator(); }
        const_iterator end() const      { return iterator(); }
        const_iterator cend() const     { return iterator(); }

        inline void reserve(SizeType sz){
            if(sz > m_bucketSize){
                void* addr = SFAllocator<HashNode**>::allocate(sz);
                HashNode** data = static_cast<HashNode**>(addr);
                HashNode* ad2 = static_cast<HashNode*>(addr);

                cout << "Address of void* addr: " << addr << endl;
                cout << "Address of HashNode** data: " << data << endl;
                cout << "Address of HashNode* ad2: " << ad2 << endl;

                (void)ad2;
                for(SizeType i = 0; i < sz; i++)
                    data[i] = nullptr;
                SizeType counter = 0;
                for(SizeType i = 0; i < m_bucketSize; i++){
                    if(m_buckets[i]){
                        imbue_data(m_buckets[i]->key, m_buckets[i]->value, data, sz, counter);
                        for(auto link = m_buckets[i]->next; link; link = link->next)
                            imbue_data(m_buckets[i]->key, m_buckets[i]->value, data, counter);
                        delete m_buckets[i];
                    }
                }
                SFAllocator<HashNode*>::deallocate(m_buckets);
                m_nodeSize = counter;
                m_bucketSize = sz;
                m_buckets = data;
            }
        }

        inline SizeType FORCE_INLINE hash(const Key& ky, SizeType sz){
            return hash_it(ky) % sz;
        }

    private:

        struct HashNode{
            const Key key;
            Value value;
            ~HashNode() = default;
            HashNode* next;
        };

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
            SizeType usless = 0;
            return imbue_data(ky, val, mem, memSize, usless);
        }

        inline HashNode* imbue_data(const Key& ky, Value& val, HashNode** mem, SizeType memSize, SizeType& counter){
            auto index = hash(ky, memSize);
            HashNode*& node = mem[index];
            cout << "Node is " << node << endl;
            if(node){
                HashNode* link = node;
                if(node->key == ky){
                    cout << " Didn't add " << ky << " : " << val << " primary node at pos: " << hash(ky, memSize) << endl;
                    return node;
                }
                else{
                    for(link = node->next; link; link = link->next)
                        if(link->key == ky)
                            return link;
                }
                link->next = new HashNode{ ky, std::move(val), nullptr };
                ++counter;
                cout << "added " << ky << " : " << val << " secondary node at pos: " << hash(ky, memSize) << endl;
                return link->next;
            }
            node = new HashNode{ ky, std::move(val), nullptr };
            cout << "NODE ADDED KEY: key=" << ky << "     node->key=" << node->key << endl;
            //delete node;
            //node = new HashNode{ ky, std::move(val), nullptr };
            ++counter;
            cout << " added " << ky << " : " << val << " primary node at pos: " << hash(ky, memSize) << "  addr: " << node << endl;
            return node;
        }

        inline iterator FORCE_INLINE getNode(const Key& ky) const {
            SizeType idx = hash(ky, m_buckets);
            HashNode* node = m_buckets[idx];
            if(node){
                HashNode* link = node;
                if(node->key == ky){
                    return iterator(this, node, idx);
                }
                else{
                    for(link = node->next; link; link = link->next)
                        if(link->key == ky)
                            return iterator(this, link, idx);
                }
            }
            return iterator{};
        }

public:
        class iterator : public std::iterator<
                std::forward_iterator_tag,
                std::pair<Key, Value>
                >
        {
            iterator(HashMap<Key, Value>* hmp) : hashMap(hmp) {
                go_to_next(true);
            }
            iterator(HashMap<Key, Value> *hmp, HashNode* nd, SizeType index) :
                hashMap(hmp), currentNode(nd), idx(index) {
                //
            }
        public:
            iterator(){}
            ~iterator(){}
            iterator(const iterator& other) = default;
            iterator& operator = (const iterator& other) = default;
            std::pair<const Key&, Value&> operator* () {
                return {currentNode->key, currentNode->value};
            }
            std::pair<const Key&, const Value&> operator* () const {
                return {currentNode->key, currentNode->value};
            }
            iterator& operator ++ () const {
                go_to_next();
                return *this;
            }
            iterator& operator ++ (int) const {
                iterator tmp(*this);
                go_to_next();
                return tmp;
            }

            friend bool operator == (const iterator& lhs, const iterator& rhs){
                return lhs.currentNode == rhs.currentNode;
            }
            friend bool operator != (const iterator& lhs, const iterator& rhs){
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
                            currentNode = hashMap->m_buckets[idx];
                            return currentNode;
                        }
                    }
                else
                    currentNode = currentNode->next ? currentNode->next : go_to_next(false);
                return nullptr;

            }
        };
};

#endif // HASHMAP_H
