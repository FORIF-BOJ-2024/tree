#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>

using namespace std;

// B+ Tree class
template <typename T>
class BPlusTree
{
public:
    // Structure to create a node
    struct Node
    {
        bool isLeaf;
        vector<T> keys;
        vector<Node *> children; // For internal nodes
        Node *next;              // For leaf nodes

        Node(bool leaf = false)
            : isLeaf(leaf), next(nullptr) {}
    };

    Node *root;
    int t; // Minimum degree

    // Constructor
    BPlusTree(int degree)
        : root(nullptr), t(degree) {}

    // Destructor
    ~BPlusTree()
    {
        destroyTree(root);
    }

    // Public member functions
    void insert(T key);
    bool search(T key);
    void remove(T key);
    vector<T> rangeQuery(T lower, T upper);
    void printTree();

private:
    // Helper member functions
    void splitChild(Node *parent, int index, Node *child);
    void insertNonFull(Node *node, T key);
    bool searchInternal(Node *node, T key);
    void removeInternal(Node *node, T key);
    void borrowFromPrev(Node *node, int index);
    void borrowFromNext(Node *node, int index);
    void mergeNodes(Node *node, int index);
    void printTreeHelper(Node *node, int level);
    void destroyTree(Node *node);
};

// Implementation of splitChild function
template <typename T>
void BPlusTree<T>::splitChild(Node *parent, int index, Node *child)
{

}

// Implementation of insertNonFull function
template <typename T>
void BPlusTree<T>::insertNonFull(Node *node, T key)
{

}

// Implementation of insert function
template <typename T>
void BPlusTree<T>::insert(T key)
{
 
}

// Implementation of search function
template <typename T>
bool BPlusTree<T>::search(T key)
{
    return searchInternal(root, key);
}

// Helper search function
template <typename T>
bool BPlusTree<T>::searchInternal(Node *node, T key)
{

}

// Implementation of range query function
template <typename T>
vector<T> BPlusTree<T>::rangeQuery(T lower, T upper)
{
 
}

// Implementation of remove function
template <typename T>
void BPlusTree<T>::remove(T key)
{
 
}

// Helper remove function
template <typename T>
void BPlusTree<T>::removeInternal(Node *node, T key)
{

}

// Borrow a key from the previous sibling
template <typename T>
void BPlusTree<T>::borrowFromPrev(Node *node, int index)
{
 
}

// Borrow a key from the next sibling
template <typename T>
void BPlusTree<T>::borrowFromNext(Node *node, int index)
{

}

// Merge two children nodes
template <typename T>
void BPlusTree<T>::mergeNodes(Node *node, int index)
{

}

// Implementation of printTree function
template <typename T>
void BPlusTree<T>::printTree()
{
    printTreeHelper(root, 0);
}

// Helper function to print the tree
template <typename T>
void BPlusTree<T>::printTreeHelper(Node *node, int level)
{
    if (node == nullptr)
    {
        return;
    }

    // Indentation based on level
    for (int i = 0; i < level; ++i)
    {
        cout << "    ";
    }

    // Indicate node type
    if (node->isLeaf)
    {
        cout << "[Leaf] ";
    }
    else
    {
        cout << "[Internal] ";
    }

    // Print keys
    for (const T &key : node->keys)
    {
        cout << key << " ";
    }
    cout << endl;

    // Recurse for children
    if (!node->isLeaf)
    {
        for (Node *child : node->children)
        {
            printTreeHelper(child, level + 1);
        }
    }
}

// Helper function to destroy the tree and free memory
template <typename T>
void BPlusTree<T>::destroyTree(Node *node)
{

}

int main()
{
    // Define the minimum degree (t)
    int degree = 2;
    BPlusTree<int> tree(degree);

    // Insert elements
    // "F", "O", "R", "IF", "HAC", "KAT", "HON", "MERRY", "CHRIST", "MAS~~!!"
    vector<int> keysToInsert = {70, 79, 82, 143, 204, 224, 229, 399, 461, 543};
    cout << "Inserting keys: ";
    for (int key : keysToInsert)
    {
        cout << key << " ";
        tree.insert(key);
    }
    cout << "\n\nB+ Tree after insertions:" << endl;
    tree.printTree();

    // Attempt to insert duplicate keys
    cout << "\nAttempting to insert duplicate key 20:" << endl;
    tree.insert(20);

    // Search for existing and non-existing keys
    vector<int> searchKeys = {15, 25, 60, 80};
    for (int key : searchKeys)
    {
        cout << "\nSearching for key " << key << ": "
             << (tree.search(key) ? "Found" : "Not Found") << endl;
    }

    Remove keys
    vector<int> keysToRemove = {};
    for (int key : keysToRemove)
    {
        cout << "\nRemoving key " << key << "..." << endl;
        tree.remove(key);
        cout << "B+ Tree after removing " << key << ":" << endl;
        tree.printTree();
    }

    // Final range query to check the tree's integrity
    cout << "\nFinal Range query [10, 70]: ";
    vector<int> finalRange = tree.rangeQuery(10, 1000);
    for (int key : finalRange)
    {
        cout << key << " ";
    }
    cout << endl;

    return 0;
}
