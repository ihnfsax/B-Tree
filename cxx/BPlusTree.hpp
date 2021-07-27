#pragma once
#include <functional>
#include <iostream>

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

protected:
    class InnerNode {  // 内部节点
    public:
        InnerNode*  parent = nullptr;  // 父亲
        Key*        key    = nullptr;  // 关键码数组
        InnerNode** child  = nullptr;  // 孩子数组
        order_type  count  = 0;
        InnerNode(order_type m) {
            key   = new Key[m];
            child = new InnerNode*[m];
        }
        ~InnerNode() {
            delete[] key;
            delete[] child;
        }
    };

    class LeafNode : public InnerNode {  // 叶子节点
    public:
        T* data = nullptr;  // 数据数组
        LeafNode(order_type m) : InnerNode(m) {
            data = new T[m];
        }
        ~LeafNode() {
            delete[] data;
        }
    };

    int        _size = 0;
    int        _order;
    InnerNode* _root;

public:
};
}  // namespace my