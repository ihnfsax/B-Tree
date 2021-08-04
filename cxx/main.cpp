#include "BPlusTree.hpp"
#include "Serialization.hpp"
#include "TestBPlusTree.hpp"
#include <iostream>
#include <map>
#include <string>
#include <sys/stat.h>
#include <vector>

#define RWRWRW (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

using namespace my;
using namespace std;

void printMode(const struct stat* statbuf) {
    printf("File Permissions: \t");
    printf((S_ISDIR(statbuf->st_mode)) ? "d" : "-");
    printf((statbuf->st_mode & S_IRUSR) ? "r" : "-");
    printf((statbuf->st_mode & S_IWUSR) ? "w" : "-");
    printf((statbuf->st_mode & S_IXUSR) ? "x" : "-");
    printf((statbuf->st_mode & S_IRGRP) ? "r" : "-");
    printf((statbuf->st_mode & S_IWGRP) ? "w" : "-");
    printf((statbuf->st_mode & S_IXGRP) ? "x" : "-");
    printf((statbuf->st_mode & S_IROTH) ? "r" : "-");
    printf((statbuf->st_mode & S_IWOTH) ? "w" : "-");
    printf((statbuf->st_mode & S_IXOTH) ? "x" : "-");
    printf("\n");
}

int main(int argc, char* argv[]) {

    // BPlusTree<int, string>* btree = new BPlusTree<int, string>(5);
    // btree->clear();
    // cout << btree->insert(1, "123") << "\n";
    // cout << btree->insert(3, "hell") << "\n";
    // cout << btree->insert(-1, "before") << "\n";
    // btree->print();
    // BPlusTree<int, string> btree3;
    // btree3 = *btree;
    // btree3.print();
    // btree3.find(3)->second = "zzzz";
    // cout << btree->insert(0, "dmq") << "\n";
    // cout << btree->insert(2, "doom") << "\n";
    // cout << btree->insert(8, "poll") << "\n";
    // cout << btree->insert(9, "fdsk") << "\n";
    // cout << btree->insert(9, "dff") << "\n";
    // cout << btree->insert(10, "kfk") << "\n";
    // cout << btree->insert(11, "kkrqk") << "\n";
    // cout << btree->insert(13, "qek") << "\n";
    // cout << btree->insert(4, "kvg") << "\n";
    // cout << btree->insert(6, "aff") << "\n";
    // cout << btree->insert(-4, "yet") << "\n";
    // cout << btree->insert(-7, "trw") << "\n";
    // cout << btree->insert(-2, "qr") << "\n";
    // cout << btree->insert(5, "tqer") << "\n";
    // cout << btree->insert(7, "iuy") << "\n";
    // cout << btree->insert(15, "op") << "\n";
    // cout << btree->insert(16, "jhf") << "\n";
    // cout << btree->insert(12, "rwe") << "\n";
    // cout << btree->insert(17, "nvm") << "\n";
    // cout << btree->insert(19, "fda") << "\n";
    // cout << btree->insert(18, "qwq") << "\n";
    // btree->print();
    // BPlusTree<int, string> btree2(*btree);
    // btree2.print();
    // delete btree;
    // btree2.print();
    // auto it1 = btree2.findMax();
    // if (it1 != btree2.end()) {
    //     cout << it1->first << ":" << it1->second << "\n";
    // }
    // auto it2 = btree2.findMax(-3);
    // if (it2 != btree2.end()) {
    //     cout << it2->first << ":" << it2->second << "\n";
    // }
    // auto it3 = btree2.findMin(-7);
    // if (it3 != btree2.end()) {
    //     cout << it3->first << ":" << it3->second << "\n";
    // }
    // auto it4 = btree2.findMin(20);
    // if (it4 != btree2.end()) {
    //     cout << it4->first << ":" << it4->second << "\n";
    // }
    // TestBPlusTree test;
    // test.checkAll();
    // struct stat s;
    // stat("./", &s);
    // printMode(&s);
    BPlusTree<string, string> btree(7);
    for (int i = 0; i < 100; ++i) {
        btree.insert(to_string(i), "aaaa");
    }
    btree.print();
    Serialization::serialization(btree, "./btree.bpt", RWRWRW);
    BPlusTree<string, string>* btree2 = Serialization::deserialization<string, string>("./btree.bpt");
    btree2->print();
    delete btree2;
    return 0;
}