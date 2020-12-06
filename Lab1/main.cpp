#include <iostream>
#include <algorithm>
#include <vector>
#include <functional>

using namespace std;

class Node {
public:

    explicit Node(char key) : key(key), height(1), left(nullptr), right(nullptr) {}

    ~Node() {
        delete left;
        delete right;
    }

    char key;
    int height;
    Node *left;
    Node *right;
};

class AVLTree {
private:

    static int height(Node *node);

    Node *insert(Node *node, char key);

    Node *find(Node *node, char key);

    static Node *leftRotate(Node *node);

    static Node *rightRotate(Node *node);

    static Node *leftRightRotate(Node *node);

    static Node *rightLeftRotate(Node *node);

public:
    ~AVLTree() {
        delete root;
    }

    Node *root = nullptr;

    void InsertElement(char key);

    pair<char *, char *> getChildrenNodesValues(char *key = nullptr);

    char *getRootNode();

    string PreOrderTraversal() const;

    string PostOrderTraversal() const;
};

int height(Node *node) {
    if (node == nullptr) {
        return 0;
    }

    return node->height;
}

int balance(Node *node) {
    if (node == nullptr) return 0;
    return height(node->left) - height(node->right);
}

void AVLTree::InsertElement(char key) {
    root = insert(root, key);
}

Node *AVLTree::insert(Node *node, char key) {
    if (node == nullptr) {
        node = new Node(key);
        return node;
    }

    if (key < node->key) {
        node->left = insert(node->left, key);
    } else if (key > node->key) {
        node->right = insert(node->right, key);
    } else {
        return node;
    }

    node->height = 1 + max(height(node->left), height(node->right));
    int bal = balance(node);

    if (bal > 1 && key < node->left->key) {
        return rightRotate(node);
    }

    if (bal < -1 && key > node->right->key)
        return leftRotate(node);

    if (bal > 1 && key > node->left->key) {
        return leftRightRotate(node);
    }

    if (bal < -1 && key < node->right->key) {
        return rightLeftRotate(node);
    }

    return node;
}

pair<char *, char *> AVLTree::getChildrenNodesValues(char *key) {
    Node *node = find(root, *key);
    if (node == nullptr) {
        return {nullptr, nullptr};
    }

    return {&node->left->key, &node->right->key};
}

Node *AVLTree::find(Node *node, char key) {
    if (key < node->key) {
        return find(node->left, key);
    }
    if (key > node->key) {
        return find(node->right, key);
    }
    return nullptr;
}

char *AVLTree::getRootNode() {
    if (this->root == nullptr) return nullptr; else return &this->root->key;
}

int AVLTree::height(Node *node) {
    if (node == nullptr) {
        return 0;
    }
    return node->height;
}

// ============== ROTATIONS ==============
Node *AVLTree::rightRotate(Node *node) {
    Node *left = node->left;
    Node *left_right = left->right;

    left->right = node;
    node->left = left_right;

    node->height = 1 + max(height(node->left), height(node->right));
    left->height = 1 + max(height(left->left), height(left->right));

    return left;
}

Node *AVLTree::leftRotate(Node *node) {
    Node *right = node->right;
    Node *left = right->left;

    right->left = node;
    node->right = left;

    node->height = 1 + max(height(node->left), height(node->right));
    right->height = 1 + max(height(right->left), height(right->right));

    return right;
}

Node *AVLTree::leftRightRotate(Node *node) {
    node->left = leftRotate(node->left);
    return rightRotate(node);
}

Node *AVLTree::rightLeftRotate(Node *node) {
    node->right = rightRotate(node->right);
    return leftRotate(node);
}

// ============== ORDERS ==============
void preOrder(Node *node, vector<string> &accumulator) {
    if (node == nullptr) {
        return;
    }

    accumulator.emplace_back(1, node->key);
    preOrder(node->left, accumulator);
    preOrder(node->right, accumulator);
}

void postOrder(Node *node, vector<string> &accumulator) {
    if (node == nullptr) {
        return;
    }

    postOrder(node->left, accumulator);
    postOrder(node->right, accumulator);
    accumulator.emplace_back(1, node->key);
}

vector<string> traversal(Node *root, const function<void(Node *, vector<string> &)> &order) {
    vector<string> accumulator = vector<string>();
    order(root, accumulator);
    return accumulator;
}

string toString(const vector<string> &keys) {
    if (keys.empty()) { return ""; }
    std::string res;
    res += keys.at(0);
    for (int i = 1; i < keys.size(); ++i) {
        res += keys.at(i);
    }
    return res;
}

string AVLTree::PreOrderTraversal() const {
    return toString(traversal(root, preOrder));
}

string AVLTree::PostOrderTraversal() const {
    return toString(traversal(root, postOrder));
}

int main() {
    string input = "123456789";
    AVLTree avl = AVLTree();
    for (auto c : input) {
        avl.InsertElement(c);
    }

    cout << ("421365879" == avl.PreOrderTraversal()) << endl;
    cout << ("132579864" == avl.PostOrderTraversal()) << endl;

    return 0;
}
