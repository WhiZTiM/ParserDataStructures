#include <list>
#include <memory>
#include <vector>
#include <unordered_set>
#include "Graph.hpp"


template<typename T, template<typename...> ContainerType = std::list>
class Groups{
public:
    using iterator = typename ContainerType::iterator;
    
    class Group{
    public:

        Group(const T& t) : data(t) {}
        Group(T&& t) : data(std::move(t)) {}
        Group(Group&&) = default;
        Group& operator = (Group&&) = default;
        ~Group() = default;

        class GroupInfo{
            using DJiterator = iterator;    //use outter name
        public:
            using iterator = std::list<DJiterator>::iterator;
            using const_iterator = std::list<DJiterator>::const_iterator;
            std::list<Group*> members;

            iterator end() { return members.end(); }
            iterator begin() { return members.begin(); }
            const_iterator end() const { return members.end(); }
            const_iterator begin() const { return members.begin(); }
            const_iterator cend() const { return members.cend(); }
            const_iterator cbegin() const { return members.cbegin(); }
        };


        bool includes(iterator iter) const {
            return id == iter->get_group_id();
        }

        int get_id() const {
            return id;
        }

        T& get_data() {
            return data;
        }

    private:
        T data;
        std::unique_ptr<SetInfo> info;
        int id;
    };


    const Group& get_group(const U& u);

    void merge(iterator a, iterator b);

    iterator insert(const T& t);
    iterator insert(T&& t);

private:
    std::list<Group> groups;
};
