#pragma once
#include <iostream>

/* const imply internal linkage in C++ */
/* new feature of C++17: "inline" allow to use a single memory address for each constant */
inline constexpr int treeSize[]  = { 10,      100,     1000,    10000,   40000,   70000,   100000,
                                    400000,  700000,  1000000, 2000000, 3000000, 4000000, 5000000,
                                    6000000, 7000000, 8000000, 9000000, 10000000 };
inline constexpr int orderSize[] = { 8,   16,   32,   64,   128,  256,  384,  512,  640,  768,
                                     896, 1024, 1152, 1280, 1408, 1536, 1664, 1792, 1920, 2048 };

namespace my {
template <class Key, class T> class BPlusTree;
}

class TestBPlusTree {
private:
    const size_t tmidx = sizeof(treeSize) / sizeof(int) - 1;
    const size_t omidx = sizeof(orderSize) / sizeof(int) - 1;
    FILE*        fp    = stdout;
    TestBPlusTree(const TestBPlusTree& t) {}

public:
    TestBPlusTree(const char* filePath = nullptr) {
        if (filePath == nullptr)
            fp = stdout;
        else
            fp = fopen(filePath, "w");
    }
    ~TestBPlusTree() {
        if (fp && fp != stdout) {
            fclose(fp);
        }
    }
    void   testAll(const int& stage = 0);
    void   measureTimeComplexity();
    void   measureEffectOfOrder(const int& scale);
    void   measureSerialize();
    void   measurePerformance();
    double testInsert(my::BPlusTree<int, std::string>& btree);
    double testErase(my::BPlusTree<int, std::string>& btree);

private:
    /* retrun: Microsecond */
    double getTimeDifference(const struct timespec& t2, const struct timespec& t1) const;
};
