#include "BPlusTree.hpp"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    // my::BPlusTree<int, std::string>::BTNode node(5, true);
    // node.key[0]                                 = 1;
    // node.key[1]                                 = 10;
    // node.key[2]                                 = 100;
    // my::BPlusTree<int, std::string>::ListPtr d1 = new my::BPlusTree<int, std::string>::ListNode(&node.key[0], "A");
    // my::BPlusTree<int, std::string>::ListPtr d2 = new my::BPlusTree<int, std::string>::ListNode(&node.key[1], "B");
    // my::BPlusTree<int, std::string>::ListPtr d3 = new my::BPlusTree<int, std::string>::ListNode(&node.key[2], "C");
    // d1->next                                    = d2;
    // d1->prior                                   = nullptr;
    // d2->next                                    = d3;
    // d2->prior                                   = d1;
    // d3->prior                                   = d2;
    // d3->next                                    = nullptr;
    // node.entry[0]                               = d1;
    // node.entry[1]                               = d2;
    // node.entry[2]                               = d3;
    // node.count                                  = 3;
    // std::cout << node.search(1) << "\n";
    // std::cout << node.search(8) << "\n";
    // std::cout << node.search(80) << "\n";
    // std::cout << node.search(100) << "\n";
    // std::cout << node.search(102) << "\n";
    // std::cout << node.search(-1) << "\n";
    // std::cout << node.search(0) << "\n";
    // node.insert(0, 5, "hello");
    // node.insert(-1, -1, "ukpkmkk");
    // node.insert(4, 130, "hhh");
    // my::BPlusTree<int, std::string>::ListPtr p = node.entry[0];
    // while (p) {
    //     std::cout << *(p->key) << ": " << p->data << "\n";
    //     p = p->next;
    // }
    my::BPlusTree<int, std::string> btree(5);
    std::cout << btree.insert(1, "123") << "\n";
    std::cout << btree.insert(3, "hell") << "\n";
    std::cout << btree.insert(-1, "before") << "\n";
    std::cout << btree.insert(2, "doom") << "\n";
    std::cout << btree.insert(8, "poll") << "\n";
    std::cout << btree.insert(9, "kkk") << "\n";
    std::cout << btree.insert(9, "kkk") << "\n";
    std::cout << btree.insert(10, "kkk") << "\n";
    std::cout << btree.insert(11, "kkk") << "\n";
    std::cout << btree.insert(13, "kkk") << "\n";
    std::cout << btree.insert(4, "kkk") << "\n";
    std::cout << btree.insert(6, "kkk") << "\n";
    std::cout << btree.insert(-4, "kkk") << "\n";
    std::cout << btree.insert(-7, "kkk") << "\n";
    std::cout << btree.insert(-2, "kkk") << "\n";
    std::cout << btree.insert(5, "kkk") << "\n";
    std::cout << btree.insert(7, "kkk") << "\n";
    std::cout << btree.insert(15, "kkk") << "\n";
    std::cout << btree.insert(16, "kkk") << "\n";
    std::cout << btree.insert(17, "kkk") << "\n";
    // std::cout << btree.insert(18, "kkk") << "\n";
    btree.insert(0, "new");
    btree.print();
    std::cout << btree.size() << "\n";
    std::cout << btree.ncount() << "\n";
    std::cout << btree.erase(-7) << "\n";
    btree.print();
    std::cout << btree.size() << "\n";
    std::cout << btree.ncount() << "\n";

    std::cout << btree.erase(1) << "\n";
    btree.print();
    std::cout << btree.size() << "\n";
    std::cout << btree.ncount() << "\n";

    std::cout << btree.erase(0) << "\n";
    btree.print();
    std::cout << btree.size() << "\n";
    std::cout << btree.ncount() << "\n";

    std::cout << btree.erase(15) << "\n";
    btree.print();
    std::cout << btree.size() << "\n";
    std::cout << btree.ncount() << "\n";
    return 0;
}