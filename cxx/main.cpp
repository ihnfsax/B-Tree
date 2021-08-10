#include "BPlusTree.hpp"
#include "Serialization.hpp"
#include "TestBPlusTree.hpp"
#include <iostream>
#include <string>
#include <sys/stat.h>

using namespace my;
using namespace std;

int main(int argc, char* argv[]) {
    BPlusTree<int, string> btree(5);  // order is set to 5 and the default is 3
    if (!btree.insert(0, "Chris"))
        cout << "0 already exists!\n";
    if (!btree.insert(pair<int, string>(0, "Chris")))
        cout << "0 already exists!\n";
    if (!btree.erase(0))
        cout << "there is no A!\n";
    if (btree.empty())
        cout << "btree is empty!\n";
    for (int i = 0; i < 20; ++i) {
        btree.insert(i, to_string(i * 2));
    }
    cout << "number of data is " << btree.size();
    cout << "\nnumber of node is " << btree.ncount();
    auto it = btree.find(10);  // search key
    if (it != btree.end()) {
        cout << "\nkey: " << (*it).first << " data: " << it->second;
        it->second = "cccc";  // modify data
    }
    auto it2 = btree.find(10);
    if (it2 != btree.end())
        cout << "\nkey: " << (*it2).first << " data: " << it2->second;
    cout << "\nrange search: >= 15 ";
    auto it3 = btree.findMin(15);
    for (; it3 != btree.end(); ++it3) {
        cout << "\nkey: " << it3->first << " data: " << it3->second;
    }
    cout << "\nrange search: < 5 ";
    auto it4 = btree.findMax(5);
    if (it4->first != 5) {
        ++it4;
    }
    auto it5 = btree.begin();
    for (; it5 != it4; ++it5) {
        cout << "\nkey: " << it5->first << " data: " << it5->second;
    }
    auto it6 = btree.findMax();
    cout << "\nlargest: key: " << it6->first << " data: " << it6->second << "\n";
    btree.print();                           // DEBUG
    BPlusTree<int, string>  btree2 = btree;  // copy-constructor
    BPlusTree<int, string>* btree3 = new BPlusTree<int, string>();
    *btree3                        = btree;  // operator =
    btree.clear();
    btree3->print();
    delete btree3;  // destrcutor
    // serialize and deserialize
    Serialization serialization;
    serialization.serialize(btree2, "btree.bpt");
    BPlusTree<int, string>* btree4 = serialization.deserialize<int, string>("btree.bpt");
    btree4->print();
    delete btree4;
    // Test
    TestBPlusTree test;  //  test(filePath): you can specify an output file instead of stdout
    test.testAll();
    return 0;
}