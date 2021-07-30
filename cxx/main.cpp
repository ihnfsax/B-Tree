#include "BPlusTree.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace my;
using namespace std;

int main(int argc, char* argv[]) {
    BPlusTree<int, string> btree(5);
    cout << btree.insert(1, "123") << "\n";
    cout << btree.insert(3, "hell") << "\n";
    cout << btree.insert(-1, "before") << "\n";
    cout << btree.insert(2, "doom") << "\n";
    cout << btree.insert(8, "poll") << "\n";
    cout << btree.insert(9, "kkk") << "\n";
    cout << btree.insert(9, "kkk") << "\n";
    cout << btree.insert(10, "kkk") << "\n";
    cout << btree.insert(11, "kkk") << "\n";
    cout << btree.insert(13, "kkk") << "\n";
    cout << btree.insert(4, "kkk") << "\n";
    cout << btree.insert(6, "kkk") << "\n";
    cout << btree.insert(-4, "kkk") << "\n";
    cout << btree.insert(-7, "kkk") << "\n";
    cout << btree.insert(-2, "kkk") << "\n";
    cout << btree.insert(5, "kkk") << "\n";
    cout << btree.insert(7, "kkk") << "\n";
    cout << btree.insert(15, "kkk") << "\n";
    cout << btree.insert(16, "kkk") << "\n";
    cout << btree.insert(17, "kkk") << "\n";
    // cout << btree.insert(18, "kkk") << "\n";
    btree.insert(0, "new");
    btree.print();
    cout << btree.size() << "\n";
    cout << btree.ncount() << "\n";
    cout << btree.erase(-7) << "\n";
    btree.print();
    cout << btree.size() << "\n";
    cout << btree.ncount() << "\n";

    cout << btree.erase(1) << "\n";
    btree.print();
    cout << btree.size() << "\n";
    cout << btree.ncount() << "\n";

    cout << btree.erase(0) << "\n";
    btree.print();
    cout << btree.size() << "\n";
    cout << btree.ncount() << "\n";

    cout << btree.erase(15) << "\n";
    btree.print();
    cout << btree.size() << "\n";
    cout << btree.ncount() << "\n";
    auto it1 = btree.begin();
    auto it2 = btree.begin();
    ++it1;
    it2 = it1;
    btree.print();
    return 0;
}