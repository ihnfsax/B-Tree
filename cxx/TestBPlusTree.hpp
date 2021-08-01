#pragma once
#include "BPlusTree.hpp"
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <time.h>

#define DEFAULT_ORDER 256
#define STRING_SIZE 1
#define MAX_SIZE 10000000

const int roundSize[] = { 10, 100, 1000, 10000, 100000, 1000000 };  // 10000000, 20000000, 30000000, 40000000 };
const int orderSize[] = { 8, 16, 32, 64, 256, 512, 1024, 2048 };

using namespace my;
using namespace std;

class TestBPlusTree {
public:
    TestBPlusTree() {
        srand(time(NULL));
    };

    void checkAll() {
        int scale = 1000000;
        cout << "Stage 1 : check class validity (" << scale << " entries, " << DEFAULT_ORDER << " keys per node) \n";
        checkValidity(scale);
        cout << "Stage 2 : measure time complexity (" << DEFAULT_ORDER << " keys per node) \n";
        measureTimeComplexity();
        int scale2 = 1000000;
        cout << "Stage 3 : measure effect of order (" << scale2 << " entries) \n";
        measureEffectOfOrder(scale2);
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

    void measureTimeComplexity() {
        BPlusTree<int, string> btree(DEFAULT_ORDER);
        for (int roundIdx = 0; roundIdx < sizeof(roundSize) / sizeof(int); ++roundIdx) {
            auto checkpoint1 = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < roundSize[roundIdx]; ++i) {
                btree.insert(i, string(STRING_SIZE, 'a'));
            }
            auto checkpoint2 = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < roundSize[roundIdx]; ++i) {
                btree.erase(i);
            }
            auto                          checkpoint3 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> insertTime  = checkpoint2 - checkpoint1;
            std::chrono::duration<double> eraseTime   = checkpoint3 - checkpoint2;
            cout << "      ├── scale: " << setw(9) << roundSize[roundIdx] << "  insert: " << setw(13)
                 << insertTime.count() << "s erase: " << eraseTime.count() << "s\n";
        }
        cout << "      └── finished\n";
    }

    void measureEffectOfOrder(const int& scale) {
        BPlusTree<int, string>* btree = nullptr;
        for (int orderIdx = 0; orderIdx < sizeof(orderSize) / sizeof(int); ++orderIdx) {
            btree            = new BPlusTree<int, string>(orderSize[orderIdx]);
            auto checkpoint1 = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < scale; ++i) {
                btree->insert(i, string(STRING_SIZE, 'a'));
            }
            auto checkpoint2 = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < scale; ++i) {
                btree->find(i);
            }
            auto checkpoint3 = std::chrono::high_resolution_clock::now();
            delete btree;
            std::chrono::duration<double> insertTime = checkpoint2 - checkpoint1;
            std::chrono::duration<double> eraseTime  = checkpoint3 - checkpoint2;
            cout << "      ├── order: " << setw(9) << orderSize[orderIdx] << "  insert: " << setw(13)
                 << insertTime.count() << "s find: " << eraseTime.count() << "s\n";
        }
        cout << "      └── finished\n";
    }
};