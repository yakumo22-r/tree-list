#pragma once
#include <vector>
#include <stack>

// TODO .h + .cpp version

/*
Tree structure, using multiple memory blocks to store data and linking all data in the form of a linked list.


Relationship links: information about the sibling nodes before and after each node, parent node, and the last child node.

Position links: information about the previous and next nodes in terms of position for each node.

Using relationship links to insert or delete elements under a specified node, elements can also be indexed through paths.

Using position links allows for an ordered traversal of the entire tree.

the max size of nodes: 65535
*/

namespace internal
{
class TreeListBase
{
public:
    using Destructor = void(*)(void * p);

    /* { 0x??: blockId | 0x??: index }  */
    enum class Addr : uint16_t { Null = 0xffff };

    struct Node final
    {
        // relationship link
        Addr parent, lastChild, left, right;

        // location link
        Addr next, prev;
    };

    template<typename V>
    class Stack final
    {
    public:
        // using V = int;
        V PopBack()
        {
            --size;
            return _block[size];
        }

        void Push(const V & v)
        {
            if (size >= capacity)
            {
                if (size == 0)
                {
                    Reserve(8);
                }
                else
                {
                    Reserve(size * 2);
                }
            }
            _block[size] = v;
            ++size;
        }

        void Reserve(uint32_t capacity)
        {
            V * t_nBlock = new V[capacity];
            memcpy(t_nBlock, _block, size * sizeof(V));
            delete[] _block;
            _block = t_nBlock;
            this->capacity = capacity;
        }
        bool Empty()const { return size == 0; }
        uint32_t Size()const { return size; }
        uint32_t Capacity()const { return capacity; }
    private:
        V * _block = nullptr;
        uint32_t size = 0;
        uint32_t capacity = 0;
    };

protected:
    Addr get_new_addr()
    {
        if (unused.Empty()) expand_block();
        ++size;
        return unused.PopBack();
    }

    void expand_block()
    {
        uint16_t tBlockId = 0;
        uint32_t tExpandSize = 0;
        if (blockSize == 256)
        {
            tExpandSize = 256;
            blocks.emplace_back(malloc(tExpandSize * (sizeof(Node) + valueSize)));
            tBlockId = blocks.size() - 1;
        }
        else if (blocks.size() == 1)
        {
            tBlockId = 0;
            tExpandSize = blockSize;
            blockSize *= 2;

            // copy data to large memmory
            void * t_block = malloc((blockSize + tExpandSize) * (sizeof(Node) + valueSize));
            memcpy(t_block, blocks.front(), blockSize * (sizeof(Node) + valueSize));
            free(blocks.front());
            blocks.front() = t_block;

            blockSize += tExpandSize;
        }
        else
        {
            // create first block
            tExpandSize = blockSize;
            blocks.push_back(malloc(tExpandSize * (sizeof(Node) + valueSize)));
            tBlockId = 0;
        }

        uint8_t tIndex = blockSize - 1;
        for (uint32_t i = 0; i < tExpandSize; ++i)
        {
            unused.Push(static_cast<Addr>((tBlockId << 8) | tIndex));
            --tIndex;
        }
    }

    Node * node(Addr addr)
    {
        if (addr == Addr::Null) { return nullptr; }
        return (Node *)((char *)blocks[static_cast<uint16_t>(addr) >> 8]
            + (static_cast<uint16_t>(addr) & 0xff)
            * (sizeof(Node) + valueSize));
    }

    void * value_ptr(Addr addr)
    {
        return ((char *)blocks[static_cast<uint16_t>(addr) >> 8])
            + (static_cast<uint16_t>(addr) & 0xff)
            * (sizeof(Node) + valueSize) + sizeof(Node);
    }

    Addr node_tail(Addr addr)
    {
        Addr tail = addr;
        Node * t_tailNode = node(tail);
        while (t_tailNode->lastChild != Addr::Null)
        {
            tail = t_tailNode->lastChild;
            t_tailNode = node(tail);
        }
        return tail;
    }

    void link_insert_front(Addr element, Addr elementTail, Addr parent = Addr::Null)
    {
        Node * t_elementNode = node(element);
        Node * t_parentNode = node(parent);

        Node * t_elementTailNode;
        if (element == elementTail)
        {
            /* init element */
            t_elementNode->lastChild = Addr::Null;

            t_elementTailNode = t_elementNode;
        }
        else
        {
            t_elementTailNode = node(elementTail);
        }


        /* relationship link */
        t_elementNode->parent = parent;
        t_elementNode->left = Addr::Null;
        t_elementNode->prev = parent;


        if (t_parentNode)
        {
            t_elementTailNode->next = t_parentNode->next;

            t_parentNode->next = element;
            if (t_parentNode->lastChild == Addr::Null)
            {
                t_parentNode->lastChild = element;
                t_elementNode->right = Addr::Null;
            }
            else
            {
                t_elementNode->right = t_parentNode->next;
            }
        }
        else // insert at tree root
        {
            t_elementNode->right = left;
            t_elementTailNode->next = left;

            Node * t_rightNode = node(t_elementNode->right);
            if (t_rightNode == nullptr)// tree is empty
            {
                right = element;
                tail = elementTail;
            }

            left = element;

        }

        if (Node * t_rightNode = node(t_elementNode->right))
        {
            t_rightNode->left = element;
            t_rightNode->prev = elementTail;
        }
    }

    // insert {element} after {location}
    void link_insert_after(Addr element, Addr elementTail, Addr left, Addr leftTail)
    {
        Node * t_elementNode = node(element);
        Node * t_leftNode = node(left);

        if (t_leftNode == nullptr)
        {
            link_insert_front(element, elementTail);
        }
        else
        {
            Node * t_elementTailNode;
            if (element == elementTail)
            {
                t_elementTailNode = t_elementNode;

                /* init element */
                t_elementNode->lastChild = Addr::Null;
            }
            else
            {
                t_elementTailNode = node(elementTail);
            }

            /* relationship link */
            t_elementNode->parent = t_leftNode->parent;
            t_elementNode->left = left;
            t_elementNode->right = t_leftNode->right;
            t_leftNode->right = element;

            if (Node * t_rightNode = node(t_elementNode->right))
            {
                t_rightNode->left = element;

                /* location link */
                t_rightNode->prev = elementTail;
            }

            /* location link */
            node(leftTail)->next = element;
            t_elementNode->prev = leftTail;
            t_elementTailNode->next = t_elementTailNode->right;

            if (t_elementTailNode->parent == Addr::Null)
            {
                this->right = element;
                this->tail = elementTail;
            }
        }
    }

    // must link again or erase after invoke this function
    void disconnect_link(Addr element, Addr elementTail)
    {
        Node * t_elementNode = node(element);
        Node * t_elementTailNode = node(elementTail);

        /* disconnect in parent */
        Node * t_parent = node(t_elementNode->parent);
        if (t_parent && t_parent->lastChild == element)
        {
            t_parent->lastChild = t_elementNode->prev;
        }

        if (Node * t_rightNode = node(t_elementNode->right))
        {
            t_rightNode->left = t_elementNode->left;
        }
        if (Node * t_leftNode = node(t_elementNode->left))
        {
            t_leftNode->right = t_elementNode->right;
        }
        if (Node * t_prevNode = node(t_elementNode->prev))
        {
            t_prevNode->next = t_elementTailNode->next;
        }
        if (Node * t_nextNode = node(t_elementTailNode->next))
        {
            t_nextNode->prev = t_elementNode->prev;
        }
    }

    // put addr to {unused} & erase children
    void erase_element(Addr element)
    {
        Node * t_elementNode = node(element);

        // erase element
        --size;
        unused.Push(element);
        node(element)->~Node();
        _valueDestructor(value_ptr(element));

        Addr next = t_elementNode->next;
        // erase children in element
        while (next != t_elementNode->right)
        {
            --size;
            unused.Push(next);
            _valueDestructor(value_ptr(element));

            next = node(next)->next;
        }
    }

public:
    class ElementBase
    {
    protected:
        ElementBase() :
            _list(nullptr),
            addr(Addr::Null),
            rightAddr(Addr::Null),
            prevAddr(Addr::Null)
        {

        }
        ElementBase(TreeListBase * list, Addr addr) :
            _list(list),
            addr(addr),
            rightAddr(Addr::Null),
            prevAddr(Addr::Null)
        { }

        mutable TreeListBase * _list;
        Addr addr;
        Addr rightAddr;
        Addr prevAddr;

        Node * node(Addr addr)const { return _list->node(addr); }
        void * value_ptr(Addr addr)const { return _list->value_ptr(addr); }
        Addr node_tail(Addr addr)const { return _list->node_tail(addr); }
        Addr get_new_addr()const { return _list->get_new_addr(); }
        void link_insert_front(Addr element, Addr elementTail, Addr parent = Addr::Null)const
        {
            _list->link_insert_front(element, elementTail, parent);
        }
        void link_insert_after(Addr element, Addr elementTail, Addr left, Addr leftTail)const
        {
            _list->link_insert_after(element, elementTail, left, leftTail);
        }
    public:
        bool operator==(const ElementBase & right)const
        {
            return addr == right.addr;
        }
        bool operator!=(const ElementBase & right)const
        {
            return addr != right.addr;
        }
        bool HasParent()const
        {
            if (Node * t_node = node(addr))
            {
                return t_node->parent != Addr::Null;
            }
            return false;
        }
        bool HasChildren()const
        {
            if (Node * t_node = node(addr))
            {
                return t_node->lastChild != Addr::Null;
            }
            return false;
        }
        bool HasRight()const
        {
            if (Node * t_node = node(addr))
            {
                return t_node->right != Addr::Null;
            }
            return false;
        }


        operator bool() const { return addr == Addr::Null; }

        // TODO: error check
        void DestorySelf()
        {
            _list->disconnect_link(addr, node_tail(addr));
            _list->erase_element(addr);
            addr = Addr::Null;
        }

        //TODO: error check
        void InsertAfter(ElementBase & element)const
        {
            Addr elementTail = node_tail(element.addr);
            _list->disconnect_link(element.addr, elementTail);
            _list->link_insert_after(element.addr, elementTail, addr, node_tail(addr));
        }

        //TODO: error check
        void InsertChildBack(ElementBase & element)const
        {
            Addr elementTail = node_tail(element.addr);
            _list->disconnect_link(element.addr, elementTail);
            Addr left = node(addr)->lastChild;
            if (left == Addr::Null)
            {
                _list->link_insert_front(element.addr, elementTail, addr);
            }
            else
            {
                _list->link_insert_after(element.addr, elementTail, left, node_tail(left));
            }
        }

        //TODO: error check
        void InsertChildFront(ElementBase & element)const
        {
            Addr elementTail = node_tail(element.addr);
            _list->disconnect_link(element.addr, elementTail);
            _list->link_insert_front(element.addr, elementTail, addr);
        }
    public:
        template<typename E>
        friend class ChildrenGroup;
    };
    template<typename E>
    class ChildrenGroup final
    {
    public:
        ChildrenGroup(ElementBase & element) : front(element) { }
        E begin()const { return E(front); }
        E end()const { return E(ElementBase(front, front.addr)); }
    private:
        ElementBase front;
    };
    friend class ElementBase;
public:
    ~TreeListBase()
    {
        Addr tAddr = left;
        while (tAddr != Addr::Null)
        {
            _valueDestructor(value_ptr(tAddr));
            tAddr = node(tAddr)->next;
        }
        for (void * _block : blocks)
        {
            free(_block);
        }
        blocks.clear();
    }
protected:
    TreeListBase(uint32_t valueSize, Destructor valueDestructor) :
        blocks(),
        unused(),
        _valueDestructor(valueDestructor),
        valueSize(valueSize),
        blockSize(8),
        size(0),
        left(Addr::Null),
        right(Addr::Null),
        tail(Addr::Null)
    { }

    std::vector<void *> blocks;
    Stack<Addr> unused;
    Destructor _valueDestructor;
    uint32_t valueSize;
    uint16_t blockSize;
    uint16_t size;

    // the first node
    Addr left;

    // the last node without parent
    Addr right;

    // the last node
    Addr tail;
};
}

template<typename T>
class TreeList final : public internal::TreeListBase
{
public:
private:
    struct _Ty final
    {
        T t;

        _Ty(const T & t) : t(t) { };
        _Ty(T && t) : t(std::move(t)) { };
    };
public:
    class Element final : public internal::TreeListBase::ElementBase
    {
    public:
        Element(TreeListBase * list, Addr addr) :
            internal::TreeListBase::ElementBase(list, addr)
        {

        }

        T & operator*() { return *(T *)value_ptr(addr); }
        Element & operator++()
        {
            if (addr != Addr::Null)
            {
                addr = node(addr)->next;
                if (addr == rightAddr) addr = Addr::Null;
            }
            return *this;
        }
        Element & operator--()
        {
            if (addr != Addr::Null)
            {
                addr = node(addr)->prev;
                if (addr == prevAddr) addr = Addr::Null;
            }
            return *this;
        }

        // TODO: error check
        template<typename R>
        Element InsertNewChild(R && r)
        {
            Element child(_list, get_new_addr());
            new (value_ptr(child.addr)) T(std::forward<T>(r));

            auto t_node = node(child.addr);
            t_node->parent = Addr::Null;
            t_node->lastChild = Addr::Null;
            t_node->left = Addr::Null;
            t_node->right = Addr::Null;
            t_node->prev = Addr::Null;
            t_node->next = Addr::Null;

            InsertChildBack(child);
            return child;
        }

        // TODO: error check
        template<typename R>
        Element InsertNewAfter(R && r)
        {
            Element child(_list, get_new_addr());
            new (value_ptr(child.addr)) T(std::forward<T>(r));

            InsertAfter(child);
            return child;
        }

        ChildrenGroup<Element> Children()
        {
            Element e(_list, Addr::Null);

            if (Node * t_node = node(addr);
                t_node->lastChild != Addr::Null)
            {
                e.addr = t_node->next;
                e.rightAddr = t_node->right;
                e.prevAddr = t_node->prev;
            }
            return ChildrenGroup<Element>(e);
        }

        Element Parent()
        {
            return Element(_list, node(addr)->parent);
        }
    };
public:
    TreeList() :
        internal::TreeListBase(sizeof(T), [](void * p) { ((_Ty *)p)->~_Ty(); })
    {

    }

    // TODO: error check
    template<typename R>
    Element InsertNewFront(R && r)
    {
        Addr tAddr = get_new_addr();
        new (value_ptr(tAddr)) T(std::forward<T>(r));
        link_insert_front(tAddr, tAddr);
        return Element(this, tAddr);
    }

    // TODO: error check
    template<typename R>
    Element InsertNewBack(R && r)
    {
        Addr tAddr = get_new_addr();
        new (value_ptr(tAddr)) T(std::forward<T>(r));
        link_insert_after(tAddr, tAddr, right, tail);
        return Element(this, tAddr);
    }

    Element begin() { return Element(this, left); }
    Element end() { return Element(this, Addr::Null); }
    Element front() { return Element(this, left); }
    Element back() { return Element(this, tail); }
};