#pragma once
#include <functional>
#include <iostream>
#include <stdexcept>

namespace my {
template <class T1, class T2> struct Pair {
    typedef T1 first_type;
    typedef T2 second_type;
    T1         first;
    T2         second;
    Pair() : first(T1()), second(T2()){};
    Pair(const T1& t1, const T2& t2) : first(t1), second(t2){};
};

template <class Key, class T, class Compare = std::less<Key>> class BPlusTree {
public:
    typedef Key                key_type;
    typedef T                  data_type;
    typedef Pair<const Key, T> value_type;
    typedef short              order_type;
    typedef unsigned int       size_type;

protected:
    typedef unsigned int node_size_type;  // no need bigger than size_type
    typedef struct ListNode {
        data_type        data;
        struct ListNode* prior;
        struct ListNode* next;
    } ListNode, *ListPtr;

    class BTIterator {
    private:
        ListPtr node;

    public:
        BTIterator(ListPtr n) : node(n){};

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
        key_type*  key   = nullptr;
        BTNode**   child = nullptr;
        ListPtr*   entry = nullptr;
        order_type count = 0;
        BTNode(order_type m, bool type) : type(type) {
            key = new key_type[m];
            if (type) {
                entry = new ListPtr[m];
            }
            else {
                child = new BTNode*[m];
            }
        }
        ~BTNode() {
            delete[] key;
            if (type) {
                delete[] entry;
            }
            else {
                delete[] child;
            }
        }
    };

    size_type      _size       = 0;  // 数据数量
    node_size_type _node_count = 0;  // 节点数量
    order_type     _order;
    BTNode*        _root;
    ListPtr        _head;

public:
    BPlusTree(order_type order = 3) : _order(order) {
        if (order < 0) {
            throw std::runtime_error("order of BPlusTree can not be a negative number");
        }
    };

    size_type size() const {
        return _size;
    }

    order_type order() const {
        return _order;
    }

    BTIterator find(const key_type& k) {}
};
}  // namespace my