#ifndef HASHMAP_H
#define HASHMAP_H

#include <memory>
#include <vector>
#include <forward_list>
#include "FVector.hpp"
#include "Config.hpp"

namespace detail {

    template<typename Key, typename Value>
    struct HashNode{
        Key key;
        Value value;
        std::forward_list<HashNode> next;
    };
}

template<typename Key, typename Value>
class HashMap
{
    public:
        HashMap();
        ~HashMap();
        HashMap(const HashMap& other);
        HashMap& operator=(const HashMap& other);

    private:
        FVector<detail::HashNode<Key, Value>> m_hashNode;
        SizeType m_buckets;
        SizeType m_size;
};

#endif // HASHMAP_H
