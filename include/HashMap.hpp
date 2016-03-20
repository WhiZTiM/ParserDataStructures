#ifndef HASHMAP_H
#define HASHMAP_H

#include <memory>
#include <vector>
#include <cstring>
#include <forward_list>
#include "FVector.hpp"
#include "Config.hpp"

template<typename T>
inline SizeType FORCE_INLINE hash_it(const T& t){
    return std::hash<T>()(t);
}

template<typename Key, typename Value>
class HashMap
{
    public:
        HashMap();
        ~HashMap();
        HashMap(HashMap&& other) noexcept {
            m_buckets = other.m_buckets;
            m_bucketSize = other.m_bucketSize;
            m_nodeSize = other.m_nodeSize;
        }
        HashMap(const HashMap& other);
        HashMap& operator=(HashMap&& other);
        HashMap& operator=(const HashMap& other);

        inline SizeType FORCE_INLINE size(){
            return m_nodeSize();
        }

        void insert(std::pair<Key, Value>&& kv){
            (void)imbue_data(kv.first, kv.second, m_buckets, m_bucketSize);
        }

        inline void reserve(SizeType sz){
            if(sz > m_bucketSize){
                HashNode** data = static_cast<HashNode**>(SFAllocator<HashNode*>::allocate(sz));
                std::memset(data, 0, sz * sizeof(HashNode*));
                SizeType counter = 0;
                for(SizeType i = 0; i < m_bucketSize; i++){
                    if(m_buckets[i]){
                        bool added = false;
                        (void)imbue_data(m_buckets[i]->key, m_buckets[i]->value, data, sz, counter);
                        for(const auto& iter : m_buckets[i]->next)
                            (void)imbue_data(m_buckets[i]->key, m_buckets[i]->value, data, counter);
                        delete m_buckets[i];
                    }
                }
                SFAllocator<HashNode*>::dellocate(m_buckets);
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
            Key key;
            Value value;
            std::forward_list<HashNode> next;
        };

        HashNode** m_buckets = nullptr;
        SizeType m_bucketSize = 0;
        SizeType m_nodeSize = 0;

        inline HashNode* FORCE_INLINE inbue_data(Key& ky, Value& val, HashNode** mem, SizeType memSize){
            SizeType usless = 0;
            return inbue_data(ky, val, mem, memSize, usless);
        }

        inline HashNode* FORCE_INLINE inbue_data(Key& ky, Value& val, HashNode** mem, SizeType memSize, SizeType& counter){
            HashNode* node = mem[hash(ky, memSize)];
            if(node){
                if(node->key == ky){
                    return node;
                }
                else{
                    for(const auto& iter : node->next)
                        if(iter.key == ky)
                            return &iter;
                }
                auto rtn = node->next.emplace_after( node->next.before_begin(), HashNode());
                ++counter;
                return rtn;
            }
            node = new HashNode{ std::move(ky), std::move(val), std::forward_list<HashNode>() };
            ++counter;
            return node;
        }

        inline HashNode* FORCE_INLINE getNode(const Key& ky){
            HashNode* node = m_buckets[hash(ky, m_buckets)];
            if(node){
                if(node->key == ky){
                    return node;
                }
                else{
                    for(const auto& iter : node->next)
                        if(iter.key == ky)
                            return &iter;
                }
            }
            return nullptr;
        }
};

#endif // HASHMAP_H
