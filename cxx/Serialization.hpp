/* non-portable, not consider endianness */
#pragma once
#include "BPlusTree.hpp"
// #include <concepts>
#include <fcntl.h>
#include <map>
#include <typeinfo>
#include <unistd.h>

#define TP_UNKNOWN 0x00    // unknown type
#define TP_INT 0x01        // int
#define TP_STLSTRING 0x02  // std::string
#define MAGIC 0xBF52

using namespace std;
namespace my {

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
    off_t  offset;
    size_t totalKeySize;
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
    static bool serialization(const BPlusTree<Key, T>& btree, const char* filePath, const mode_t& mode) {
        int fd;
        if ((fd = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, mode | S_IRUSR | S_IWUSR)) == -1)
            return false;
        BptHeader h;
        if (writeHeader(fd, btree, h) == false)
            return false;
        map<BTNode<Key, T>*, FNode> nodeMap;

        return true;
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
    template <class Key, class T> static bool writeHeader(const int& fd, const BPlusTree<Key, T>& btree, BptHeader& h) {
        h.magic = MAGIC;
        if ((h.keyType = getType<Key>()) == TP_UNKNOWN) {
            throw std::runtime_error("unknown key type");
        }
        if ((h.dataType = getType<T>()) == TP_UNKNOWN) {
            throw std::runtime_error("unknown data type");
        }
        h.orderSize = (char)sizeof(order_type<Key, T>);
        h.sizeSize  = (char)sizeof(size_type<Key, T>);
        h.offSize   = (char)sizeof(off_t);
        char* h2;
        if ((h2 = new char[2 * h.sizeSize + h.orderSize]) == nullptr) {
            return false;
        }
        memcpy(h2, &btree._order, h.orderSize);
        memcpy(h2 + h.orderSize, &btree._size, h.sizeSize);
        memcpy(h2 + h.orderSize + h.sizeSize, &btree._node_count, h.sizeSize);
        if (write(fd, &h, sizeof(BptHeader)) == -1) {
            return false;
        }
        if (write(fd, h2, 2 * h.sizeSize + h.orderSize) == -1) {
            return false;
        }
        delete[] h2;
        return true;
    }

    template <class Key, class T> static void writeNode(const int& fd, const BTNode<Key, T>* const n);
};
}  // namespace my