#pragma once
#include "BPlusTree.hpp"
#include "Serialization.hpp"
#include <random>
#include <sys/time.h>

#define REPEAT 200
#define STRING_SIZE 1
#define DEFAULT_ORDER 64

using namespace my;
using namespace std;

const int treeSize[]  = { 10,     100,    1000,    10000,   40000,   70000,    100000,
                         400000, 700000, 1000000, 4000000, 7000000, 10000000, 30000000 };
const int orderSize[] = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048 };

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

    void testAll(const int& stage = 0) {
        if (stage == 1 || stage <= 0 || stage > 5) {
            fprintf(fp, "Stage 1 : measure time complexity (%d keys per node) \n", DEFAULT_ORDER);
            measureTimeComplexity();
        }
        if (stage == 2 || stage <= 0 || stage > 5) {
            int scale = 2000000;
            fprintf(fp, "Stage 2 : measure effect of order (%d entries) \n", scale);
            measureEffectOfOrder(scale);
        }
        if (stage == 3 || stage <= 0 || stage > 5) {
            fprintf(fp, "Stage 3 : measure serialization and deserialization cost (%d keys per node)\n", DEFAULT_ORDER);
            mearsureSerialize();
        }
    }

    void measureTimeComplexity() {
        fflush(fp);
        uniform_int_distribution<int> dist;
        random_device                 rd;
        default_random_engine         rng{ rd() };
        BPlusTree<int, string>        btree(DEFAULT_ORDER);
        double                        insertTime, eraseTime;
        for (size_t tidx = 0; tidx <= tmidx;) {
            if (btree.size() == treeSize[tidx]) {
                insertTime = testInsert(btree);
                eraseTime  = testInsert(btree);
                fprintf(fp, "      ├── size: %8d  ncount: %7d  height: %2d  insert: %9.6lfus  erase: %9.6lfus\n",
                        btree.size(), btree.ncount(), btree.height(), insertTime, eraseTime);
                fflush(fp);
                tidx++;
            }
            btree.insert(dist(rng), string(STRING_SIZE, 'a'));
        }
        fprintf(fp, "      └── finished\n");
        fflush(fp);
    }

    void measureEffectOfOrder(const int& scale) {
        fflush(fp);
        uniform_int_distribution<int> dist;
        random_device                 rd;
        default_random_engine         rng{ rd() };
        double                        insertTime, eraseTime;
        for (size_t oidx = 0; oidx <= omidx; ++oidx) {
            BPlusTree<int, string> btree(orderSize[oidx]);
            while (btree.size() < scale) {
                btree.insert(dist(rng), string(STRING_SIZE, 'a'));
            }
            insertTime = testInsert(btree);
            eraseTime  = testInsert(btree);
            fprintf(fp, "      ├── order: %4d  ncount: %6d  height: %2d  insert: %10.6lfus  erase: %10.6lfus\n",
                    btree.order(), btree.ncount(), btree.height(), insertTime, eraseTime);
            fflush(fp);
        }
        fprintf(fp, "      └── finished\n");
        fflush(fp);
    }

    void mearsureSerialize() {
        fflush(fp);
        uniform_int_distribution<int> dist;
        random_device                 rd;
        default_random_engine         rng{ rd() };
        BPlusTree<int, string>        btree(DEFAULT_ORDER);
        double                        serializeTime, deserializeTime;
        off_t                         fileSize;
        struct timespec               t1, t2;
        char                          filePath[] = "./testXXXXXX";
        int                           fd;
        if ((fd = mkstemp(filePath)) < 0)
            throw runtime_error("mkstemp error");
        close(fd);
        for (size_t tidx = 0; tidx <= tmidx;) {
            if (btree.size() == treeSize[tidx]) {
                clock_gettime(CLOCK_REALTIME, &t1);
                fileSize = Serialization::serialize(btree, filePath);
                clock_gettime(CLOCK_REALTIME, &t2);
                serializeTime = getTimeDifference(t2, t1);
                clock_gettime(CLOCK_REALTIME, &t1);
                BPlusTree<int, string>* btreePtr = Serialization::deserialize<int, string>(filePath);
                clock_gettime(CLOCK_REALTIME, &t2);
                deserializeTime = getTimeDifference(t2, t1);
                delete btreePtr;
                fprintf(fp,
                        "      ├── size: %8d  ncount: %6d  height: %2d  serialize: %10.4lfms  deserialize: %10.4lfms  "
                        "file size: %9ldB\n",
                        btree.size(), btree.ncount(), btree.height(), serializeTime / 1000, deserializeTime / 1000,
                        fileSize);
                fflush(fp);
                tidx++;
            }
            btree.insert(dist(rng), string(STRING_SIZE, 'a'));
        }
        unlink(filePath);
        fprintf(fp, "      └── finished\n");
        fflush(fp);
    }

    double testInsert(BPlusTree<int, string>& btree) {
        double                        insertTime;
        struct timespec               t1, t2;
        uniform_int_distribution<int> dist;
        random_device                 rd;
        default_random_engine         rng{ rd() };
        for (int i = 0; i < REPEAT; ++i) {
            int rand = dist(rng);
            clock_gettime(CLOCK_REALTIME, &t1);
            bool flag = btree.insert(rand, string(STRING_SIZE, 'a'));
            clock_gettime(CLOCK_REALTIME, &t2);
            if (flag) {
                insertTime += getTimeDifference(t2, t1);
                btree.erase(rand);
            }
        }
        return insertTime / REPEAT;
    }

    double testErase(BPlusTree<int, string>& btree) {
        double                        eraseTime;
        struct timespec               t1, t2;
        uniform_int_distribution<int> dist{ 0, (int)btree.size() - 1 };
        random_device                 rd;
        default_random_engine         rng{ rd() };
        for (int i = 0; i < REPEAT; ++i) {
            int  rand = dist(rng);
            auto it   = btree.begin();
            while (rand--) {
                ++it;
            }
            int    key  = it->first;
            string data = it->second;
            clock_gettime(CLOCK_REALTIME, &t1);
            btree.erase(key);
            clock_gettime(CLOCK_REALTIME, &t2);
            eraseTime += getTimeDifference(t2, t1);
            btree.insert(key, data);
        }
        return eraseTime / REPEAT;
    }

private:
    /* retrun: Microsecond */
    double getTimeDifference(const struct timespec& t2, const struct timespec& t1) const {
        double secDiff  = (static_cast<double>(t2.tv_sec) - static_cast<double>(t1.tv_sec)) * 1000000;
        double nsecDiff = (static_cast<double>(t2.tv_nsec) - static_cast<double>(t1.tv_nsec)) / 1000;
        return secDiff + nsecDiff;
    }
};