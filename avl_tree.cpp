#include <iterator>
#include <vector>
#include <algorithm>

template<class T>
class Set {
private:
    class Node {
    public:
        T key;
        int height;
        Node* left, *right, *parent;

        explicit Node(const T& value) : key(value), height(1), left(nullptr), right(nullptr), parent(nullptr) {
            key = value;
        }

        static int node_height(const Node* node) {
            if (node == nullptr) {
                return 0;
            } else {
                return node->height;
            }
        }

        static int get_balance(const Node* p) {
            return node_height(p->right) - node_height(p->left);
        }

        static void update_parent(Node* children, Node* parent) {
            if (children == nullptr)
                return;
            children->parent = parent;
        }

        static void update(Node* node) {
            if (node == nullptr)
                return;

            update_parent(node->left, node);
            update_parent(node->right, node);
            node->height = std::max(node_height(node->left), node_height(node->right)) + 1;
        }

        static Node* right_rotation(Node* node) {
            if (node == nullptr)
                return nullptr;

            Node* temp = node->left;

            node->left = temp->right;
            if (node->left)
                node->left->parent = node;

            temp->right = node;
            if (temp->right)
                temp->right->parent = temp;

            update(temp);
            update(node);

            return temp;
        }

        static Node* left_rotation(Node* node) {
            if (node == nullptr)
                return nullptr;

            Node* temp = node->right;

            node->right = temp->left;
            if (node->right)
                node->right->parent = node;

            temp->left = node;
            if (temp->left)
                temp->left->parent = temp;

            update(temp);
            update(node);

            return temp;
        }

        static Node* balance(Node* node) {
            if (node == nullptr)
                return nullptr;

            update(node);

            if (get_balance(node) == -2) {
                if (get_balance(node->left) > 0)
                    node->left = left_rotation(node->left);
                return right_rotation(node);
            }

            if (get_balance(node) == 2) {
                if (get_balance(node->right) < 0)
                    node->right = right_rotation(node->right);
                return left_rotation(node);
            }

            return node;
        }

        static Node* find(Node* node, const T& value) {
            if (node == nullptr)
                return nullptr;

            if (value < node->key) {
                return find(node->left, value);
            } else if (node->key < value) {
                return find(node->right, value);
            } else {
                return node;
            }
        }

        static Node* lower_bound(Node* node, const T& value) {
            if (node == nullptr)
                return nullptr;

            if (node->key < value) {
                return lower_bound(node->right, value);
            } else {
                Node* temp = lower_bound(node->left, value);
                if (temp && temp->key < node->key) {
                    return temp;
                } else {
                    return node;
                }
            }
        }

        static Node* insert(Node* node, const T& value) {
            if (node == nullptr)
                return new Node(value);

            if (value < node->key) {
                node->left = insert(node->left, value);
            } else if (node->key < value) {
                node->right = insert(node->right, value);
            }

            return balance(node);
        }

        static Node* get_min(Node* node) {
            Node* temp = node;

            while (temp && temp->left != nullptr)
                temp = temp->left;

            return temp;
        }

        static Node* get_max(Node* node) {
            Node* temp = node;

            while (temp && temp->right != nullptr)
                temp = temp->right;

            return temp;
        }

        static Node* remove_min(Node* node) {
            if (node == nullptr)
                return nullptr;

            if (node->left == nullptr)
                return node->right;

            node->left = remove_min(node->left);
            return balance(node);
        }

        static Node* remove(Node* node, const T& value)  {
            if (node == nullptr)
                return nullptr;

            if (value < node->key) {
                node->left = remove(node->left, value);
            } else if (node->key < value) {
                node->right = remove(node->right, value);
            } else {
                Node* l = node->left;
                Node* r = node->right;

                delete node;

                if (r == nullptr)
                    return l;

                Node* min = get_min(r);
                min->right = remove_min(r);
                min->left = l;

                return balance(min);
            }
            return balance(node);
        }

        static Node* next(Node* node) {
            if (node == nullptr)
                return node;

            if (node->right) {
                Node *temp = node->right;
                while (temp->left != nullptr)
                    temp = temp->left;
                return temp;
            }

            Node* temp = node;

            while (temp->parent && temp->parent->right == temp) {
                temp = temp->parent;
            }

            return temp->parent;
        };

        static Node* prev(Node* node) {
            if (node == nullptr)
                return node;

            if (node->left) {
                Node *temp = node->left;
                while (temp->right != nullptr)
                    temp = temp->right;
                return temp;
            }

            Node* temp = node;

            while (temp->parent && temp->parent->left == temp) {
                temp = temp->parent;
            }

            return temp->parent;
        }
    };

public:
    class iterator {
        Node* iter = nullptr;
        Node* root = nullptr;

    public:
        iterator(Node* node, Node* tree): iter(node), root(tree) {
        }

        iterator() = default;

        iterator& operator = (const iterator& other) {
            iter = other.iter;
            root = other.root;
            return *this;
        }

        iterator& operator ++ () {
            iter = Node::next(iter);
            return *this;
        }

        const iterator operator ++ (int) {
            iterator temp = iterator(iter, root);
            iter = Node::next(iter);
            return temp;
        }

        iterator& operator -- () {
            if (iter == nullptr)
                iter = Node::get_max(root);
            else
                iter = Node::prev(iter);
            return *this;
        }

        const iterator operator -- (int) {
            iterator temp = iterator(iter, root);
            if (iter == nullptr)
                iter = Node::get_max(root);
            else
                iter = Node::prev(iter);
            return temp;
        }

        bool operator == (const iterator& other) const {
            return iter == other.iter;
        }

        bool operator != (const iterator& other) const {
            return iter != other.iter;
        }

        const T& operator * () const {
            return iter->key;
        }

        const T* operator -> () const {
            return &iter->key;
        }

        ~iterator() = default;
    };

    Node* root = nullptr;
    int sz = 0;

    Set() = default;

    template<typename Iterator>
    Set(Iterator first, Iterator last) {
        while (first != last) {
            insert(*first++);
        }
    }

    explicit Set(const std::initializer_list<T>& elems) {
        for (const T& value : elems) {
            insert(value);
        }
    }

    Set(const Set& other) {
        for (const T& value : other) {
            insert(value);
        }
    }

    Set& operator = (const Set& other) {
        if (this == &other)
            return *this;

        del(root);
        root = nullptr;

        for (const T& value : other) {
            insert(value);
        }

        return *this;
    }

    void del(Node* node) {
        sz = 0;
        if (node == nullptr)
            return;

        del(node->left);
        del(node->right);
        delete node;
    }

    ~Set() {
        del(root);
    }

    size_t size() const {
        return sz;
    }

    bool empty() const {
        return sz == 0;
    }

    void insert(const T& value) {
        if (Node::find(root, value) == nullptr) {
            ++sz;
            root = Node::insert(root, value);
        }

        if (root != nullptr)
            root->parent = nullptr;
    }

    void erase(const T& value) {
        if (Node::find(root, value) != nullptr) {
            --sz;
            root = Node::remove(root, value);
        }

        if (root != nullptr)
            root->parent = nullptr;
    }

    void in_order() const {
        Node::in_order(root);
    }

    iterator find(const T& value) const {
        Node* temp = Node::find(root, value);
        return iterator(temp, root);
    }

    iterator lower_bound(const T& value) const {
        Node* temp = Node::lower_bound(root, value);
        return iterator(temp, root);
    }

    iterator begin() const {
        iterator temp(Node::get_min(root), root);
        return temp;
    }

    iterator end() const {
        iterator temp(nullptr, root);
        return temp;
    }
};
