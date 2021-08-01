#include "BPlusTree.hpp"
#include <cstdlib>
#include <iostream>
#include <time.h>

using namespace my;
using namespace std;

template <class Key, class T> class TestBPlusTree {
private:
    static BPlusTree<Key, T> btree;

public:
    TestBPlusTree(){};
    BPlusTree<Key, T> getBPlusTree() {
        return btree;
    }
    void checkAll() {
        cout << "Stage 1: check class validity\n";
    }

private:
    void checkValidity() {
        BPlusTree<Key, T>* btree2 = new BPlusTree<Key, T>;
    }
};