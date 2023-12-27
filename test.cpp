#include <iostream>
#include <vector>
#include "tree_list.hpp"

std::vector<int> construct_store;

std::vector<int> destruct_store;

class TestType
{
public:
    TestType() :
        id(construct_store.size())
    {
        construct_store.push_back(id);
    }

    TestType(const TestType & type) :
        id(construct_store.size())
    {
        construct_store.push_back(id);
    }

    TestType(TestType && type) :
        id(type.id)
    {
        type.id = -1;
    }
    int GetId()const { return id; }
    ~TestType()
    {
        if (id >= 0) destruct_store.push_back(id);
    }
private:
    int id;
};


int main()
{
    {
        TreeList<TestType> list;
        TestType t;
        list.InsertNewBack(TestType{});
        list.InsertNewFront(TestType{});
        list.InsertNewBack(TestType{});
        list.InsertNewBack(TestType{});
        auto e = list.InsertNewBack(TestType{});

        e.InsertNewChild(TestType{});
        e.InsertNewChild(TestType{});

        e = list.front();
        int indent = 0;

        while (e != list.end())
        {
            for (size_t i = 0; i < indent; i++) { std::cout << " "; }
            if (e.HasParent()) std::cout << "|->";
            std::cout << "{" << (*e).GetId() << "}" << std::endl;
            if (e.HasChildren())
            {
                indent++;
            }
            else if (indent > 0 && !e.HasRight())
            {
                indent--;
            }
            e.operator++();
        }
    }

    std::cout << "constructed: ";
    for (int id : construct_store)
    {
        std::cout << id << " ";
        /* code */
    }
    std::cout << std::endl;

    std::cout << "destructed: ";
    for (int id : destruct_store)
    {
        std::cout << id << " ";
        /* code */
    }
    std::cout << std::endl;
}