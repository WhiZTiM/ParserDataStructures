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
#include "String.hpp"

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



    ////////////////////////////////////////////////////////////////////////////////
    ////                                                                       /////
    ////                    BEGIN ITERATOR IMPLEMENTAION                       /////
    ////                                                                       /////
    template<bool isConst>
    class Iterator {
        using V_T_KV = std::pair<const Key, Value>;
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = std::conditional_t<isConst, std::add_const_t<V_T_KV>, V_T_KV>;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category = std::forward_iterator_tag;

        using HMap = std::conditional_t<isConst, std::add_const_t<HashMap>*, HashMap*>;
        using Node = std::conditional_t<isConst, std::add_const_t<HashNode>*, HashNode*>;

    private:
        Iterator(HMap hmp) : hashMap(hmp) {
            currentNode = go_to_next(true);
        }

        Iterator(HMap hmp, Node nd, SizeType index) :
            hashMap(hmp), currentNode(nd), idx(index) {
            //
        }

    public:
        Iterator(){}
        ~Iterator(){}
        Iterator(const Iterator& other) = default;
        Iterator& operator = (const Iterator& other) = default;

        //non-const iterators are implicitly convertible to const_iterator
        operator Iterator<true> () const { return Iterator<true>{hashMap, currentNode, idx}; }

    private:
        //convert from const_iterator to non-const iterator
        static Iterator<false> toNonConstIterator(Iterator<true> iter) {
            return Iterator<false>{ const_cast<HashMap*>(iter.hashMap),
                                    const_cast<HashNode*>(iter.currentNode),
                                    iter.idx};
        }
    public:

        reference operator* () const {
            return currentNode->data;
        }

        pointer operator -> () const {
            return &currentNode->data;
        }

        Iterator& operator ++ () {
            go_to_next();
            return *const_cast<Iterator*>(this);
        }
        Iterator operator ++ (int) {
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
        HMap hashMap = nullptr;
        Node currentNode = nullptr;
        SizeType idx = 0;

        Node go_to_next(bool consider_currentNode = true) {
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

    ////                                                                       /////
    ////                    END ITERATOR IMPLEMENTAION                         /////
    ////                                                                       /////
    ////////////////////////////////////////////////////////////////////////////////

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
        using key_type = Key;
        using mapped_type = Value;
        using value_type = std::pair<const Key&, Value>;
        using size_type = SizeType;
        using difference_type = std::ptrdiff_t;
        using hasher = void;
        using key_equal = void;
        using pointer = value_type*;
        using reference = value_type&;
        using const_pointer = const value_type*;
        using const_reference = const value_type&;

        struct node_type{
            node_type() : m_data(nullptr) {}
            node_type(node_type&& other) : m_data(other.m_data) { other.m_data = nullptr; }
            node_type& operator = (node_type&& other) {
                delete m_data;
                m_data = other.m_data;
                other.m_data = nullptr;
                return *this;
            }
            bool is_empty() const { return m_data == nullptr; }
            ~node_type() { delete m_data; }
        private:
            friend class HashMap<Key, Value>;
  
			node_type(HashNode* ptr) : m_data(ptr) {}
            HashNode* data() { return m_data; }
            HashNode* m_data;
        };

		template<typename Iter, typename NodeType> struct InsertReturnType{
			Iter position;
			bool inserted;
			NodeType node;
		};

		using insert_return_type = InsertReturnType<iterator, node_type>;

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
            return *this;
        }

        HashMap& operator=(const HashMap& other){
            if(this == &other) return *this;
            destroy();
            copy_from(other);
        }

        inline bool FORCE_INLINE empty() const {
            return m_nodeSize == 0;
        }

        inline SizeType FORCE_INLINE size() const {
            return m_nodeSize;
        }

        inline SizeType FORCE_INLINE capacity() const {
            return m_bucketSize;
        }

        template<typename... Args>
        std::pair<iterator, bool> emplace(const Key& ky, Args... args){
            return insert( std::pair<Key, Value>(
                               std::piecewise_construct,
                               std::tuple<Key>{ky},
                               std::tuple<Args...>{ std::forward<Args>(args)... }
                               )
                           );
        }

        std::pair<iterator, bool> insert(std::pair<const Key, Value>&& kv){
            grow_memory_if_needed();
            return imbue_data(kv.first, std::move(kv.second), m_buckets, m_bucketSize);
        }

		insert_return_type insert(node_type&& node){
			if(node.is_empty())
                return { end(), false, std::move(node) };
			return imbue_node(std::move(node));
		}

        Value& operator [] (const Key& ky){
            grow_memory_if_needed();
            return imbue_data(ky, Value{}, m_buckets, m_bucketSize).first->second;
        }

        iterator find(const Key& ky) {
            return getNode(ky);
        }

        const_iterator find(const Key& ky) const {
            return const_cast<HashMap*>(this)->getNode(ky);
        }

        size_type count(const Key& ky) const {
            return find(ky) != cend() ? 1 : 0;
        }

        iterator erase(const_iterator iter){
            if(iter == cend())
                return end();
            delete disconnect_node((iter++)->first);
            return iter.toNonConstIterator(iter);
        }

        node_type extract(const_iterator iter){
            if(iter == cend())
                return node_type();
            return node_type(disconnect_node(iter->first));
        }

        node_type extract(const Key& key){
            return node_type(disconnect_node(key));
        }

        SizeType erase(const Key& ky){
            auto node = disconnect_node(ky);
            if(node){
                delete node;
                return 1;
            }
            return 0;
        }

        inline void clear(){
            destroy();
            m_buckets = nullptr;
            m_bucketSize = 0;
            m_nodeSize = 0;
        }

        inline void reserve(SizeType sz){
            if(sz > m_bucketSize){
                void* addr = SFAllocator<HashNode**>::allocate(sz);
                HashNode** data = static_cast<HashNode**>(addr);

                for(SizeType i = 0; i < sz; i++)
                    data[i] = nullptr;
                SizeType counter = 0;
                for(SizeType i = 0; i < m_bucketSize; i++){
                    if(m_buckets[i]){
                        imbue_data(m_buckets[i]->data.first, std::move(m_buckets[i]->data.second), data, sz, counter);
                        for(auto link = m_buckets[i]->next; link; link = link->next)
                            imbue_data(m_buckets[i]->data.first, std::move(m_buckets[i]->data.second), data, counter);
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
            return hash_it(ky) % sz;
        }

        void swap(HashMap& other){
            std::swap(m_buckets, other.m_buckets);
            std::swap(m_bucketSize, other.m_bucketSize);
            std::swap(m_nodeSize, other.m_nodeSize);
        }

        void friend swap(HashMap& first, HashMap& second){
            first.swap(second);
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
            for(const auto& v : other)
                emplace(v.first, v.second);
        }

        inline void grow_memory_if_needed(){
            static std::vector<SizeType> prVec = EratosthenesSieve<1000000, SizeType>().sieve();

            if(m_nodeSize >= m_bucketSize * 1.5)    //load factor of  1 / 1.5  =  0.6666667
                reserve(prVec[(m_bucketSize*2)+7]); //grow by a factor of 2... Add 7
        }

        HashNode* disconnect_node(const Key& key){
            auto index = hash(key, m_bucketSize);
            auto& node = m_buckets[index];
            HashNode* rtn = nullptr;
            if(node && node->data.first == key){
                rtn = node;
                node = node->next;
				m_nodeSize--;
            }
            else if(node){
                for(auto n = node; n->next; n = n->next){
                    if(n->next->data.first == key){
                        rtn = n->next;
                        n->next = n->next->next;
						m_nodeSize--;
                        break;
                    }
                }
            }
            return rtn;
        }


        inline void destroy() noexcept {
            //cout << "Destructor: m_buckets:" << m_buckets << endl;
            for(SizeType i=0; i < m_bucketSize; i++){
                //cout << "Destructor: m_buckets[" << i << "] : " << m_buckets[i] << endl;
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

        inline std::pair<iterator, bool> imbue_data(const Key& ky, Value&& val, HashNode** mem, SizeType memSize){
            return imbue_data(ky, std::move(val), mem, memSize, m_nodeSize);
        }

        inline std::pair<iterator, bool>
        imbue_data(const Key& ky, Value&& val, HashNode** mem, SizeType memSize, SizeType& counter){
            auto index = hash(ky, memSize);
            HashNode*& node = mem[index];
            if(node){
                HashNode* link = node;
                if(node->data.first == ky){
                    return {{this, node, index}, false };
                }
                else{
                    for(link = node; link->next; link = link->next)
                        if(link->data.first == ky)
                            return {{this, link, index}, false};
                }
                link->next = new HashNode{ {ky, std::move(val)}, nullptr };
                ++counter;
                return {{this, link->next, index}, true};
            }
            node = new HashNode{ {ky, std::move(val)}, nullptr };
            ++counter;
            return {{this, node, index}, true};
        }

		insert_return_type imbue_node(node_type&& handle){
            grow_memory_if_needed();
            auto index = hash(handle.data()->data.first, m_bucketSize);
			auto& node = m_buckets[index];
			if(node){
				auto n = node;

                //check that node does not already exist
				while(true){
					if(n->data.first == handle.data()->data.first)
						return { {this, n, index}, false, std::move(handle)};
					if(!n->next)
						break;
					n = n->next;
				}
				n->next = handle.data();
				n->next->next = nullptr;
				handle.m_data = nullptr;
                m_nodeSize++;
                return { {this, n->next, index}, true, {} };
			}

			node = handle.data();
			node->next = nullptr;
			handle.m_data = nullptr;
            m_nodeSize++;
            return {{this, node, index}, true, {}};
		}

        inline iterator FORCE_INLINE getNode(const Key& ky) {
            if(m_bucketSize == 0)
                return end();
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
