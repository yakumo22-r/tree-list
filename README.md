

## Features

- **Relationship Links**: TreeList provides information about the sibling nodes before and after each node, the parent node, and the last child node.
- **Position Links**: TreeList provides information about the previous and next nodes in terms of position for each node.
- **Insertion and Deletion**: Using relationship links, elements can be inserted or deleted under a specified node. Elements can also be indexed through paths.
- **Ordered Traversal**: Using position links, an ordered traversal of the entire tree is possible.

## Usage

node: For the best integration with your project, feel free to modify the source code of TreeList to suit your specific needs and requirements.



First, include the TreeList header file in project.

```cpp
#include "tree_list.hpp"
```

Then, declare a TreeList of the

```cpp
TreeList<int> tree;
```

insert new elements at the front or back of the tree.

```cpp
auto element = tree.InsertNewFront(10); // Insert 10 at the front
auto anotherElement = tree.InsertNewBack(20); // Insert 20 at the back
```

insert new child elements to an existing element.

```cpp
auto child = element.InsertNewChild(30); // Insert 30 as a child of 10
```

traverse through the tree using the provided iterators.

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



