#pragma once
#include "BPlusTree.hpp"
#include "Serialization.hpp"
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <time.h>

#define DEFAULT_ORDER 128
#define STRING_SIZE 1
#define MAX_SIZE 10000000

const int roundSize[] = { 10, 100, 1000, 10000, 100000, 1000000, 10000000 };  // 20000000, 30000000, 40000000 };
const int orderSize[] = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048 };

using namespace my;
using namespace std;

class TestBPlusTree {
private:
    FILE* fp = stdout;

    TestBPlusTree(const TestBPlusTree& t) {}

public:
    TestBPlusTree() {
        srand(time(NULL));
    };

    ~TestBPlusTree() {
        if (fp && fp != stdout) {
            fclose(fp);
        }
    }

    void checkAll(const char* filePath = nullptr) {
        if (filePath == nullptr)
            fp = stdout;
        else
            fp = fopen(filePath, "w");
        int scale = 1000000;
        fprintf(fp, "Stage 1 : check class validity (%dentries, %d keys per node) \n", scale, DEFAULT_ORDER);
        fflush(fp);
        checkValidity(scale);
        fflush(fp);
        fprintf(fp, "Stage 2 : measure time complexity (%d keys per node) \n", DEFAULT_ORDER);
        fflush(fp);
        measureTimeComplexity();
        fflush(fp);
        int scale2 = 1000000;
        fprintf(fp, "Stage 3 : measure effect of order (%d entries) \n", scale2);
        fflush(fp);
        measureEffectOfOrder(scale2);
        fflush(fp);
        fprintf(fp,
                "Stage 4 : measure effect of scale on serialization and deserialization time cost (%d keys per node)\n",
                DEFAULT_ORDER);
        fflush(fp);
        mearsureSerializeCostWithScale();
        fflush(fp);
        int scale3 = 1000000;
        fprintf(fp, "Stage 5 : measure effect of order on serialization and deserialization time cost (%d entries)\n",
                scale3);
        fflush(fp);
        mearsureSerializeCostWithOrder(scale3);
        fflush(fp);
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
                    fprintf(fp, "      ├── checkValidity: insert error\n");
                    exit(1);
                }
            }
            else if (btree->erase(rand() % (i + 1))) {
                s--;
                if (s != btree->size()) {
                    fprintf(fp, "      ├── checkValidity: erase error\n");
                    exit(1);
                }
            }
        }
        fprintf(fp, "      ├── insert and erase function works fine\n");
        BPlusTree<int, string> btree_copy = *btree;
        fprintf(fp, "      ├── copy-constructor works fine\n");
        delete btree;
        fprintf(fp, "      └── destrcutor works fine\n");
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
            fprintf(fp, "      ├── scale: %9d  insert: %11.8lfs  erase: %11.8lfs\n", roundSize[roundIdx],
                    insertTime.count(), eraseTime.count());
            fflush(fp);
        }
        fprintf(fp, "      └── finished\n");
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
            fprintf(fp, "      ├── order: %9d  insert: %11.8lfs  erase: %11.8lfs\n", orderSize[orderIdx],
                    insertTime.count(), eraseTime.count());
            fflush(fp);
        }
        fprintf(fp, "      └── finished\n");
    }

    void mearsureSerializeCostWithScale() {
        BPlusTree<int, string> btree(DEFAULT_ORDER);
        size_t                 maxIdx = sizeof(roundSize) / sizeof(roundSize[0]) - 1;
        size_t                 next   = 0;
        for (int i = 0; i < roundSize[maxIdx]; ++i) {
            btree.insert(i, string(STRING_SIZE, 'a'));
            if (i == roundSize[next] - 1) {
                next++;
                auto                    checkpoint1 = std::chrono::high_resolution_clock::now();
                off_t                   fileSize    = Serialization::serialize(btree, "TestBPlusTree.bpt");
                auto                    checkpoint2 = std::chrono::high_resolution_clock::now();
                BPlusTree<int, string>* btree2      = Serialization::deserialize<int, string>("TestBPlusTree.bpt");
                auto                    checkpoint3 = std::chrono::high_resolution_clock::now();
                delete btree2;
                std::chrono::duration<double> serializeTime   = checkpoint2 - checkpoint1;
                std::chrono::duration<double> deserializeTime = checkpoint3 - checkpoint2;
                fprintf(fp, "      ├── scale: %9d  serialize: %11.8lfs  deserialize: %11.8lfs  file size: %10ldB\n",
                        i + 1, serializeTime.count(), deserializeTime.count(), fileSize);
                fflush(fp);
            }
        }
        fprintf(fp, "      └── finished\n");
    }

    void mearsureSerializeCostWithOrder(const int& scale) {
        BPlusTree<int, string>* btree = nullptr;
        for (int orderIdx = 0; orderIdx < sizeof(orderSize) / sizeof(int); ++orderIdx) {
            btree = new BPlusTree<int, string>(orderSize[orderIdx]);
            for (int i = 0; i < scale; ++i) {
                btree->insert(i, string(STRING_SIZE, 'a'));
            }
            auto                    checkpoint1 = std::chrono::high_resolution_clock::now();
            off_t                   fileSize    = Serialization::serialize(*btree, "TestBPlusTree.bpt");
            auto                    checkpoint2 = std::chrono::high_resolution_clock::now();
            BPlusTree<int, string>* btree2      = Serialization::deserialize<int, string>("TestBPlusTree.bpt");
            auto                    checkpoint3 = std::chrono::high_resolution_clock::now();
            delete btree;
            delete btree2;
            std::chrono::duration<double> serializeTime   = checkpoint2 - checkpoint1;
            std::chrono::duration<double> deserializeTime = checkpoint3 - checkpoint2;
            fprintf(fp, "      ├── order: %9d  serialize: %11.8lfs  deserialize: %11.8lfs  file size: %10ldB\n",
                    orderSize[orderIdx], serializeTime.count(), deserializeTime.count(), fileSize);
            fflush(fp);
        }
        fprintf(fp, "      └── finished\n");
    }
};