/* non-portable, not consider endianness */
#pragma once
#include "BPlusTree.hpp"
// #include <concepts>
#include <fcntl.h>
#include <map>
#include <typeinfo>
#include <unistd.h>

using namespace std;
namespace my {

#define TP_UNKNOWN 0x00    // unknown type
#define TP_INT 0x01        // int
#define TP_STLSTRING 0x02  // std::string
#define MAGIC 0xBEA3

#pragma pack(1)
typedef struct BptHeader {
    int16_t magic = MAGIC;
    char    keyType;
    char    dataType;
    char    orderSize;
    char    sizeSize;  // = nodeSizeSize
    char    offSize;
} BptHeader;
#pragma pack()

typedef struct FNode {
    off_t offset;
    off_t offToKeyEnd;
} FNode;

class Serialization {
private:
    template <class Key, class T> using BTNode         = typename BPlusTree<Key, T>::BTNode;
    template <class Key, class T> using order_type     = typename BPlusTree<Key, T>::order_type;
    template <class Key, class T> using size_type      = typename BPlusTree<Key, T>::size_type;
    template <class Key, class T> using node_size_type = typename BPlusTree<Key, T>::node_size_type;
    template <class Key, class T> using ListNode       = typename BPlusTree<Key, T>::ListNode;
    template <class Key, class T> using ListPtr        = typename BPlusTree<Key, T>::ListPtr;

public:
    template <class Key, class T>
    static bool serialization(const BPlusTree<Key, T>& btree, const std::string& filePath, const mode_t& mode) {
        return serialization(btree, filePath.c_str(), mode);
    }

    template <class Key, class T>
    static void serialization(const BPlusTree<Key, T>& btree, const char* filePath, const mode_t& mode) {
        int fd;
        if ((fd = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, mode | S_IRUSR | S_IWUSR)) == -1)
            throw std::runtime_error("open error");
        BptHeader h;
        writeHeader(fd, btree, h);
        map<const BTNode<Key, T>* const, FNode> nodeMap;
        writeNode<Key, T>(fd, btree._root, nodeMap, h);
    }

    template <class Key, class T> static BPlusTree<Key, T>* deserialization(const std::string& filePath) {
        return deserialization<Key, T>(filePath.c_str());
    }

    template <class Key, class T> static BPlusTree<Key, T>* deserialization(const char* filePath) {}

    template <class T> static char getType() {
        // if constexpr (same_as<T, int>) {
        //     cout << "int\n";
        // }
        // else if constexpr (same_as<T, string>) {
        //     cout << "string\n";
        // }
        if (typeid(T) == typeid(int)) {
            return TP_INT;
        }
        else if (typeid(T) == typeid(string)) {
            return TP_STLSTRING;
        }
        else {
            return TP_UNKNOWN;
        }
    }

private:
    template <class Key, class T> static void writeHeader(const int& fd, const BPlusTree<Key, T>& btree, BptHeader& h) {
        h.magic = MAGIC;
        if ((h.keyType = getType<Key>()) == TP_UNKNOWN)
            throw std::runtime_error("unknown key type");
        if ((h.dataType = getType<T>()) == TP_UNKNOWN)
            throw std::runtime_error("unknown data type");
        h.orderSize = (char)sizeof(order_type<Key, T>);
        h.sizeSize  = (char)sizeof(size_type<Key, T>);
        h.offSize   = (char)sizeof(off_t);
        char* h2;
        if ((h2 = new char[2 * h.sizeSize + h.orderSize]) == nullptr)
            throw std::runtime_error("can not allocate memory for header (part 2)");
        memcpy(h2, &btree._order, h.orderSize);
        memcpy(h2 + h.orderSize, &btree._size, h.sizeSize);
        memcpy(h2 + h.orderSize + h.sizeSize, &btree._node_count, h.sizeSize);
        if (write(fd, &h, sizeof(BptHeader)) == -1)
            throw std::runtime_error("write error: header (part 1)");
        if (write(fd, h2, 2 * h.sizeSize + h.orderSize) == -1)
            throw std::runtime_error("write error: header (part 2)");
        delete[] h2;
    }

    template <class Key, class T>
    static void writeNode(const int& fd, const BTNode<Key, T>* const n,
                          map<const BTNode<Key, T>* const, FNode>& nodeMap, const BptHeader& h) {
        if (n == nullptr)
            return;
        FNode fnode;
        fnode.offset = lseek(fd, 0, SEEK_CUR);
        if (write(fd, &n->type, 1) == -1)
            throw std::runtime_error("write error: node type");
        if (write(fd, &n->count, h.orderSize) == -1)
            throw std::runtime_error("write error: node count");
        if (n->type) {
            for (order_type<Key, T> i = 0; i < n->count; ++i) {
                writeType(fd, n->key[i]);
                writeType(fd, n->entry[i]->data);
            }
            fnode.offToKeyEnd = 0;
        }
        else {
            for (order_type<Key, T> i = 0; i < n->count; ++i) {
                writeType(fd, n->key[i]);
            }
            fnode.offToKeyEnd = lseek(fd, 0, SEEK_CUR);
            lseek(fd, h.offSize * n->count, SEEK_CUR); /* jump child array */
        }
        nodeMap.insert(pair<const BTNode<Key, T>* const, FNode>(n, fnode));
        off_t offNow = lseek(fd, 0, SEEK_CUR);
        if (n->parent != nullptr) {
            BTNode<Key, T>* p  = n->parent;
            auto            it = nodeMap.find(p);
            if (it == nodeMap.end())
                throw std::runtime_error("child node is writen before its parent");
            order_type<Key, T> pr = 0;
            while (p->child[pr] != n)
                pr++;
            lseek(fd, it->second.offToKeyEnd + pr * h.offSize, SEEK_SET);
            if ((write(fd, &fnode.offset, h.offSize)) == -1)
                throw std::runtime_error("write error: child offset");
        }
        lseek(fd, offNow, SEEK_SET);
        if (!n->type) {
            for (order_type<Key, T> i = 0; i < n->count; ++i) {
                writeNode<Key, T>(fd, n->child[i], nodeMap, h);
            }
        }
    }

    template <class T> static void writeType(const int& fd, T& value) {
        if (typeid(T) == typeid(int)) {
            if (write(fd, &value, sizeof(int)) == -1)
                throw std::runtime_error("write error: int");
        }
        else if (typeid(T) == typeid(string)) {
            string*           temp    = reinterpret_cast<string*>(&value);
            string::size_type strSize = temp->size();
            if (write(fd, &strSize, sizeof(string::size_type)) == -1)
                throw std::runtime_error("write error: string size");
            if (write(fd, temp->c_str(), strSize) == -1)
                throw std::runtime_error("write error: string");
        }
    }
};
}  // namespace my