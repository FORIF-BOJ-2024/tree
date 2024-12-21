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
    Node *newNode = new Node(child->isLeaf);
    int midIndex = child->keys.size() / 2;

    if (child->isLeaf)
    {
        newNode->keys.assign(child->keys.begin() + midIndex, child->keys.end());
        newNode->next = child->next;
        child->next = newNode;
        child->children.erase(child->children.begin() + midIndex, child->children.end());
        parent->keys.insert(parent->keys.begin() + index, newNode->keys[0]);
        parent->children.insert(parent->children.begin() + index + 1, newNode);
    }

    else
    {
        newNode->keys.assign(child->keys.begin() + midIndex + 1, child->keys.end());
        newNode->children.assign(child->children.begin() + midIndex + 1, child->children.end());
        child->children.erase(child->children.begin() + midIndex + 1, child->children.end());
        parent->keys.insert(parent->keys.begin() + index, child->keys[midIndex]);
        parent->children.insert(parent->children.begin() + index + 1, newNode);
    }
}

// Implementation of insertNonFull function
template <typename T>
void BPlusTree<T>::insertNonFull(Node *node, T key)
{
    if (node->isLeaf)
    {
        // Insert the key in the leaf node
        auto it = upper_bound(node->keys.begin(), node->keys.end(), key);
        node->keys.insert(it, key);
    }
    else
    {
        // Find the child which is going to have the new key
        int i = 0;
        while (i < node->keys.size() && key >= node->keys[i])
        {
            i++;
        }

        // Check if the found child is full
        if (node->children[i]->keys.size() == 2 * t - 1)
        {
            // Split the child
            splitChild(node, i, node->children[i]);

            // After split, the middle key of children[i] goes up and
            // children[i] is split into two. Decide which of the two
            // will have the new key
            if (key >= node->keys[i])
            {
                i++;
            }
        }
        insertNonFull(node->children[i], key);
    }
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
    if (node == nullptr)
    {
        return false;
    }

    int i = 0;
    while (i < node->keys.size() && key >= node->keys[i])
    {
        i++;
    }

    if (node->isLeaf)
    {
        if (i > 0 && node->keys[i - 1] == key)
        {
            return true;
        }
        return false;
    }

    return searchInternal(node->children[i], key);
}

// Implementation of range query function
template <typename T>
vector<T> BPlusTree<T>::rangeQuery(T lower, T upper)
{
    vector<T> result;
    if (root == nullptr)
    {
        return result;
    }

    // Traverse to the leftmost leaf where keys >= lower
    Node *current = root;
    while (!current->isLeaf)
    {
        int i = 0;
        while (i < current->keys.size() && lower >= current->keys[i])
        {
            i++;
        }
        current = current->children[i];
    }

    // Traverse the leaf nodes and collect keys in range
    while (current != nullptr)
    {
        for (const T &key : current->keys)
        {
            if (key >= lower && key <= upper)
            {
                result.push_back(key);
            }
            if (key > upper)
            {
                return result;
            }
        }
        current = current->next;
    }

    return result;
}

// Implementation of remove function
template <typename T>
void BPlusTree<T>::remove(T key)
{
    if (root == nullptr)
    {
        cout << "The tree is empty.\n";
        return;
    }

    removeInternal(root, key);

    // If the root node has 0 keys, make its first child the new root
    if (root->keys.empty())
    {
        Node *tmp = root;
        if (root->isLeaf)
        {
            root = nullptr;
        }
        else
        {
            root = root->children[0];
        }
        delete tmp;
    }
}

// Helper remove function
template <typename T>
void BPlusTree<T>::removeInternal(Node *node, T key)
{
    int idx = lower_bound(node->keys.begin(), node->keys.end(), key) - node->keys.begin();

    if (node->isLeaf)
    {
        // If the node is a leaf, remove the key directly
        if (idx < node->keys.size() && node->keys[idx] == key)
        {
            node->keys.erase(node->keys.begin() + idx);
        }
        else
        {
            cout << "Key " << key << " does not exist in the tree.\n";
        }
        return;
    }

    // If the node is an internal node
    bool flag = ((idx < node->keys.size()) && (node->keys[idx] == key));

    if (flag)
    {
        // If the key is present in the internal node
        Node *predNode = node->children[idx];
        // Find the predecessor key
        while (!predNode->isLeaf)
        {
            predNode = predNode->children[predNode->keys.size()];
        }
        T predKey = predNode->keys.back();
        node->keys[idx] = predKey;
        removeInternal(node->children[idx], predKey);
    }
    else
    {
        // The key is not present in the internal node, recurse to the appropriate child
        bool flagChild = (idx < node->children.size());

        if (flagChild && node->children[idx]->keys.size() >= t)
        {
            removeInternal(node->children[idx], key);
        }
        else
        {
            // Handle cases where the child has less than t keys
            if (idx != 0 && node->children[idx - 1]->keys.size() >= t)
            {
                borrowFromPrev(node, idx);
            }
            else if (idx != node->children.size() - 1 && node->children[idx + 1]->keys.size() >= t)
            {
                borrowFromNext(node, idx);
            }
            else
            {
                if (idx != node->children.size() - 1)
                {
                    mergeNodes(node, idx);
                }
                else
                {
                    mergeNodes(node, idx - 1);
                }
            }
            // After merge, recurse on the merged child
            if (idx < node->children.size())
            {
                removeInternal(node->children[idx], key);
            }
            else
            {
                removeInternal(node->children[idx - 1], key);
            }
        }
    }
}

// Borrow a key from the previous sibling
template <typename T>
void BPlusTree<T>::borrowFromPrev(Node *node, int index)
{
    Node *leftnodes = node->children[index];
    Node *targetnodes = node->children[index + 1];
    if (targetnodes->isLeaf)
    {
        targetnodes.keys.insert(targetnodes.keys.begin(), nodes.keys[index]);
        nodes.keys[index] = leftnodes.keys.end();
        leftnodes.keys.erase(leftnodes.keys.end())
    }
    else
    {
        targetnodes.keys.insert(targetnodes.keys.begin(), nodes.keys[index]);
        nodes.keys[index] = leftnodes.keys.end();
        leftnodes.keys.erase(leftnodes.keys.end())

            targetnodes->children.insert(targetnodes->children.begin(), leftnodes->children.end());
        delete leftnodes->children.end();
    }
    // 포인터 지우기
}

// Borrow a key from the next sibling
template <typename T>
void BPlusTree<T>::borrowFromNext(Node *node, int index)
{
    Node *targetnodes = node->children[index];
    Node *rightnodes = node->children[index + 1];

    if (targetnodes->isLeaf)
    {
        // 자식 노드가 leaf일 때
        targetnodes->keys.insert(targetnodes->keys.end(), nodes->keys[index]);
        nodes->keys[index] = rightnodes->keys.at(1);
        rightnodes->keys.erase(rightnodes->keys.begin());
    }
    else
    {
        // 자식 노드가 내부 노드일 때
        targetnodes->keys.insert(targetnodes->keys.end(), nodes->keys[index]);
        nodes->keys[index] = rightnodes->keys.at(1);
        rightnodes->keys.erase(rightnodes->keys.begin());

        targetnodes->children.insert(targetnodes->children.end(), rightnodes->children.begin());
        delete rightnodes->children.begin();
    }
    targetnodes->keys.insert(targetnodes->keys.end(), nodes->keys[index]);
    nodes->keys[index] = rightnodes->keys.at(1);
    rightnodes->keys.erase(rightnodes->keys.begin());
}

// Merge two children nodes
template <typename T>
void BPlusTree<T>::mergeNodes(Node *node, int index)
{
    // 병합 대상 노드 가져오기
    Node *leftChild = node->children[index];
    Node *rightChild = node->children[index + 1];

    // 부모 노드에서 병합 기준 키 가져오기
    T mergeKey = node->keys[index];

    // 병합: 오른쪽 노드의 키와 자식들을 왼쪽 노드에 추가

    if (!leftChild->isLeaf)
    {
        leftChild->keys.push_back(mergeKey); // 부모의 키를 추가
        leftChild->keys.insert(leftChild->keys.end(), rightChild->keys.begin(), rightChild->keys.end());
        leftChild->children.insert(leftChild->children.end(), rightChild->children.begin(), rightChild->children.end());
    }
    else
    {
        leftChild->keys.insert(leftChild->keys.end(), rightChild->keys.begin(), rightChild->keys.end());
        leftChild->next = rightChild->next;
    }

    // 부모 노드에서 키와 자식 제거
    node->keys.erase(node->keys.begin() + index);
    node->children.erase(node->children.begin() + index + 1);

    // 메모리 해제: 오른쪽 자식 노드는 더 이상 필요하므로 삭제
    delete rightChild;
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
    if (node == nullptr)
    {
        return;
    }
    for (Node i : node->children)
        destroyTree(i);
    delete node
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

    // Remove keys
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
