

# TreeList: A C++ Single-Header Library

TreeList is a single-header C++ library that provides a class template to represent tree data structures. The library uses multiple memory blocks to store data, linking all data in the form of a linked list. The maximum size of nodes is 65,535.

note: README.md is writen by chatgpt

## Features

- **Relationship Links**: TreeList provides information about the sibling nodes before and after each node, the parent node, and the last child node.
- **Position Links**: TreeList provides information about the previous and next nodes in terms of position for each node.
- **Insertion and Deletion**: Using relationship links, elements can be inserted or deleted under a specified node. Elements can also be indexed through paths.
- **Ordered Traversal**: Using position links, an ordered traversal of the entire tree is possible.

## Usage

First, include the TreeList header file in your project.

```cpp
#include "tree_list.hpp"
```

Then, declare a TreeList of the type you want to store in the tree.

```cpp
TreeList<int> tree;
```

You can insert new elements at the front or back of the tree.

```cpp
auto element = tree.InsertNewFront(10); // Insert 10 at the front
auto anotherElement = tree.InsertNewBack(20); // Insert 20 at the back
```

You can also insert new child elements to an existing element.

```cpp
auto child = element.InsertNewChild(30); // Insert 30 as a child of 10
```

You can traverse through the tree using the provided iterators.

```cpp
for(auto it = tree.begin(); it != tree.end(); ++it) {
    std::cout << *it << std::endl;
}
```

Elements can be destroyed as well.

```cpp
element.DestorySelf(); // Destroys the element
```

## Example

For a detailed usage example, please refer to the test code provided in the `test.cpp` file.

## Contributing

Contributions to the TreeList library are welcome. Please feel free to submit issues and pull requests.

## License

TreeList is open-source software licensed under the [MIT license](http://opensource.org/licenses/MIT).





