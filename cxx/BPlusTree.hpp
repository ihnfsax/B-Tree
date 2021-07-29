#pragma once
#include <deque>  // debug
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string.h>

namespace my {
template <class T1, class T2> struct Pair {
    typedef T1 first_type;
    typedef T2 second_type;
    T1         first;
    T2         second;
    Pair() : first(T1()), second(T2()){};
    Pair(const T1& t1, const T2& t2) : first(t1), second(t2){};
};

template <class Key, class T> class BPlusTree {
public:
    typedef Key                key_type;
    typedef T                  data_type;
    typedef Pair<const Key, T> value_type;
    typedef short              order_type;
    typedef unsigned int       size_type;

    // protected:
    typedef unsigned int node_size_type;  // no need bigger than size_type
    typedef struct ListNode {
        key_type*        key;
        data_type        data;
        struct ListNode* prior = nullptr;
        struct ListNode* next  = nullptr;
        ListNode();
        ListNode(key_type* k) : key(k){};
        ListNode(key_type* k, data_type d) : key(k), data(d){};
        /* flag: true: insert before, false: insert after */
        void insert(ListNode* p, bool flag) {
            if (p == nullptr)
                throw std::runtime_error("try to insert a null node");
            if (flag) {
                p->prior = this->prior;
                p->next  = this;
                if (this->prior)
                    this->prior->next = p;
                this->prior = p;
                return;
            }
            p->prior = this;
            p->next  = this->next;
            if (this->next) {
                this->next->prior = p;
            }
            this->next = p;
        }
    } ListNode, *ListPtr;

    class BTIterator {
    private:
        ListPtr node;

    public:
        BTIterator(ListPtr n) : node(n){};

        const key_type& key() {
            if (node == nullptr) {
                throw std::runtime_error("try to get key from nullptr");
            }
            return node->key;
        }

        data_type& data() {
            if (node == nullptr) {
                throw std::runtime_error("try to get data from nullptr");
            }
            return node->data;
        }

        bool operator==(const BTIterator& other) {
            if (this->node == other.node) {
                return true;
            }
            else {
                return false;
            }
        }

        bool operator++() {
            if (node == nullptr) {
                throw std::runtime_error("BPlusTree iterator not incrementable");
            }
            else {
                node = node->next;
            }
        }

        bool operator--() {
            if (node == nullptr) {
                throw std::runtime_error("BPlusTree iterator not decrementable");
            }
            else {
                node = node->prior;
            }
        }
    };

    class BTNode {
    public:
        const bool type;  // 0: inner node; 1: leaf node
        key_type*  key    = nullptr;
        BTNode**   child  = nullptr;
        ListPtr*   entry  = nullptr;
        BTNode*    parent = nullptr;
        order_type count  = 0;
        order_type order;
        BTNode(order_type m, bool type) : order(m), type(type) {
            if (m < 3) {
                throw std::runtime_error("order of BPlusTree must >= 3");
            }
            key = new key_type[m];
            if (type)
                entry = new ListPtr[m];
            else
                child = new BTNode*[m];
        }
        ~BTNode() {
            delete[] key;
            if (type)
                delete[] entry;
            else
                delete[] child;
        }

        order_type search(const key_type& k) const {
            if (count == 0)
                return -2;
            if (k < key[0])
                return -1;
            order_type i = 0;
            for (; i < count - 1; ++i) {
                if (k < key[i + 1])
                    return i;
            }
            return i;
        }

        order_type insert(const order_type& r, const key_type& k, const data_type* const d = nullptr) {
            if (type && d == nullptr)
                throw std::runtime_error("try to insert a nullptr to data field");
            for (order_type i = count - 1; i > r; --i) {
                key[i + 1] = key[i];
                if (type) { /* leaf node */
                    entry[i + 1]      = entry[i];
                    entry[i + 1]->key = &key[i + 1];
                }
                else /* inner node */
                    child[i + 1] = child[i];
            }
            count++;
            key[r + 1] = k;
            if (type) { /* leaf node */
                entry[r + 1] = new ListNode(&key[r + 1], *d);
                if (count != 1) {
                    if (r + 1 == count - 1)
                        entry[r]->insert(entry[r + 1], false); /* insert after */
                    else
                        entry[r + 2]->insert(entry[r + 1], true); /* insert before */
                }
            }
            else /* inner node */
                child[r + 1] = nullptr;
            return r + 1;
        }

        BTNode* splitSelf(order_type s) {
            if (s >= count - 1 || s < -1)
                throw std::runtime_error("can't split node");
            BTNode* node = new BTNode(order, type);
            node->count  = count - s - 1;
            memcpy(node->key, &key[s + 1], node->count * sizeof(key_type));
            if (type)
                memcpy(node->entry, &entry[s + 1], node->count * sizeof(ListPtr));
            else {
                memcpy(node->child, &child[s + 1], node->count * sizeof(BTNode*));
                for (order_type i = 0; i < node->count; ++i) {
                    node->child[i]->parent = node;
                }
            }
            count        = s + 1;
            node->parent = parent;
            return node;
        }
    };

    size_type      _size       = 0;  // 数据数量
    node_size_type _node_count = 0;  // 节点数量
    order_type     _order;
    BTNode*        _root = nullptr;
    ListPtr        _head = nullptr;

public:
    BPlusTree(order_type order = 3) : _order(order) {
        if (order < 3) {
            throw std::runtime_error("order of BPlusTree must >= 3");
        }
    };

    size_type size() const {
        return _size;
    }

    order_type order() const {
        return _order;
    }

    BTIterator find(const key_type& key) {}

    bool insert(const key_type& key, const data_type& data) {
        BTNode* v = _root;
        if (v == nullptr) {
            /* only for _root */
            v = new BTNode(_order, true);
            v->insert(-1, key, &data);
            _root = v;
            return true;
        }
        while (true) {
            order_type r = v->search(key);
            if (v->type) {
                if (r >= 0 && v->key[r] == key)
                    return false;  // 插入失败
                doInsert(v, r, key, data);
                return true;
            }
            else {
                if (r == -1)
                    v->key[0] = key, r = 0;
                v = v->child[r];
            }
        }
    }
    /* for debug */
    void print() {
        BTNode*             v = nullptr;
        std::deque<BTNode*> pnodes, cnodes;
        int                 l = 0;
        if (_root != nullptr) {
            pnodes.push_back(_root);
            while (!pnodes.empty()) {
                std::cout << "Level " << l++ << ": ";
                while (!pnodes.empty()) {
                    v = *(pnodes.begin());
                    pnodes.pop_front();
                    std::cout << "|";
                    for (order_type i = 0; i < v->count; ++i) {
                        if (v->type) {
                            std::cout << v->key[i] << ":" << v->entry[i]->data << "|";
                        }
                        else {
                            std::cout << v->key[i] << "|";
                            cnodes.push_back(v->child[i]);
                        }
                    }
                    if (!pnodes.empty()) {
                        std::cout << "-----";
                    }
                }
                std::cout << "\n";
                pnodes = cnodes;
                cnodes.clear();
            }
        }
    }

private:
    void doInsert(BTNode* n, order_type& r, const key_type& k, const data_type& d) {
        if (n->count < _order) {
            r = n->insert(r, k, &d);
        }
        else {
            order_type s  = _order / 2;
            BTNode*    n2 = nullptr;
            if (r < s) {
                n2 = n->splitSelf(s - 1);
                r  = n->insert(r, k, &d);
            }
            else {
                n2 = n->splitSelf(s);
                r  = n2->insert(r - s - 1, k, &d);
            }
            if (n->parent) {
                order_type r2 = n->parent->search(n2->key[0]);
                doInsert(n->parent, r2, n2->key[0], d);
                n->parent->child[r2] = n2;
            }
            else {
                n->parent = new BTNode(_order, false);
                n->parent->insert(-1, n->key[0]);
                n->parent->insert(0, n2->key[0]);
                n->parent->child[0] = n;
                n->parent->child[1] = n2;
                n2->parent          = n->parent;
                _root               = n->parent;
            }
        }
    }
};  // namespace my
}  // namespace my