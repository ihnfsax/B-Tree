#pragma once
#include "BPlusTree.hpp"
#include <cstdlib>
#include <time.h>

#define DEFAULT_ORDER 256
#define STRING_SIZE 1

using namespace my;
using namespace std;

class TestBPlusTree {
public:
    TestBPlusTree() {
        srand(time(NULL));
    };

    void checkAll() {
        cout << "Stage 1 : check class validity\n";
        checkValidity(10000);
    };

private:
    void checkValidity(const int& scale) {
        BPlusTree<int, string>*           btree = new BPlusTree<int, string>(DEFAULT_ORDER);
        BPlusTree<int, string>::size_type s     = 0;
        for (int i = 0; i < scale; ++i) {
            bool flag = rand() % 2 == 0;
            if (flag) {
                btree->insert(i, string(STRING_SIZE, 'a'));
                s++;
                if (s != btree->size()) {
                    cerr << "      ├── checkValidity: insert error\n";
                    exit(1);
                }
            }
            else if (btree->erase(rand() % (i + 1))) {
                s--;
                if (s != btree->size()) {
                    cerr << "      ├── checkValidity: erase error\n";
                    exit(1);
                }
            }
        }
        cout << "      ├── insert and erase function works fine\n";
        BPlusTree<int, string> btree_copy = *btree;
        cout << "      ├── copy-constructor works fine\n";
        delete btree;
        cout << "      └── destrcutor works fine\n";
    }
};