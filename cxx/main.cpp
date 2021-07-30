#include "BPlusTree.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace my;
using namespace std;

int main(int argc, char* argv[]) {
    BPlusTree<int, string>* btree = new BPlusTree<int, string>(5);
    cout << btree->insert(1, "123") << "\n";
    cout << btree->insert(3, "hell") << "\n";
    cout << btree->insert(-1, "before") << "\n";
    cout << btree->insert(0, "dmq") << "\n";
    cout << btree->insert(2, "doom") << "\n";
    cout << btree->insert(8, "poll") << "\n";
    cout << btree->insert(9, "fdsk") << "\n";
    cout << btree->insert(9, "dff") << "\n";
    cout << btree->insert(10, "kfk") << "\n";
    cout << btree->insert(11, "kkrqk") << "\n";
    cout << btree->insert(13, "qek") << "\n";
    cout << btree->insert(4, "kvg") << "\n";
    cout << btree->insert(6, "aff") << "\n";
    cout << btree->insert(-4, "yet") << "\n";
    cout << btree->insert(-7, "trw") << "\n";
    cout << btree->insert(-2, "qr") << "\n";
    cout << btree->insert(5, "tqer") << "\n";
    cout << btree->insert(7, "iuy") << "\n";
    cout << btree->insert(15, "op") << "\n";
    cout << btree->insert(16, "jhf") << "\n";
    cout << btree->insert(17, "nvm") << "\n";
    btree->print();
    delete btree;
    return 0;
}