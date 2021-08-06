/* non-portable, not consider endianness */
#pragma once
#include "BPlusTree.hpp"
// #include <concepts>
#include <fcntl.h>
#include <map>
#include <sys/stat.h>
#include <typeinfo>
#include <unistd.h>

using namespace std;
namespace my {
#define RWRWRW (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)
#define TP_UNKNOWN 0x00    // unknown type
#define TP_INT 0x01        // int
#define TP_STLSTRING 0x02  // std::string
#define MAGIC 0xBEA378FC

#pragma pack(1)
typedef struct BptHeader {
    uint32_t magic = MAGIC;
    char     keyType;
    char     dataType;
    char     orderSize;
    char     sizeSize;  // = nodeSizeSize
    char     offSize;
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
    static off_t serialize(const BPlusTree<Key, T>& btree, const std::string& filePath, const mode_t& mode = RWRWRW) {
        return serialize(btree, filePath.c_str(), mode);
    }

    template <class Key, class T>
    static off_t serialize(const BPlusTree<Key, T>& btree, const char* filePath, const mode_t& mode = RWRWRW) {
        int fd;
        if ((fd = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, mode | S_IRUSR | S_IWUSR)) == -1)
            throw std::runtime_error("open error");
        BptHeader h;
        writeHeader(fd, btree, h);
        map<const BTNode<Key, T>* const, FNode> nodeMap;
        writeNode<Key, T>(fd, btree._root, nodeMap, h);
        off_t end;
        if ((end = lseek(fd, 0, SEEK_END)) == -1)
            throw std::runtime_error("lseek error: get file size");
        if (close(fd) == -1)
            throw std::runtime_error("close error");
        return end;
    }

    template <class Key, class T> static BPlusTree<Key, T>* deserialize(const std::string& filePath) {
        return deserialize<Key, T>(filePath.c_str());
    }

    template <class Key, class T> static BPlusTree<Key, T>* deserialize(const char* filePath) {
        int fd;
        if ((fd = open(filePath, O_RDONLY)) == -1)
            throw std::runtime_error("open error");
        // lseek(fd, 0, SEEK_SET);  // maybe do nothing
        BPlusTree<Key, T>* btree = nullptr;
        BptHeader          h;
        readHeader(fd, btree, h);
        if (btree->_size != 0)
            btree->_root = readNode<Key, T>(fd, nullptr, 0, btree->_head, btree->_order);
        if (close(fd) == -1)
            throw std::runtime_error("close error");
        return btree;
    }

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
        // TODO: add type here
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
        if ((h2 = new char[3 * h.sizeSize + h.orderSize]) == nullptr)
            throw std::runtime_error("can not allocate memory for header (part 2)");
        memcpy(h2, &btree._order, h.orderSize);
        memcpy(h2 + h.orderSize, &btree._size, h.sizeSize);
        memcpy(h2 + h.orderSize + h.sizeSize, &btree._node_count, h.sizeSize);
        memcpy(h2 + h.orderSize + 2 * h.sizeSize, &btree._height, h.sizeSize);
        if (write(fd, &h, sizeof(BptHeader)) == -1)
            throw std::runtime_error("write error: header (part 1)");
        if (write(fd, h2, 3 * h.sizeSize + h.orderSize) == -1)
            throw std::runtime_error("write error: header (part 2)");
        delete[] h2;
    }

    template <class Key, class T>
    static void writeNode(const int& fd, const BTNode<Key, T>* const n,
                          map<const BTNode<Key, T>* const, FNode>& nodeMap, const BptHeader& h) {
        if (n == nullptr)
            return;
        FNode fnode;
        if ((fnode.offset = lseek(fd, 0, SEEK_CUR)) == -1)
            throw std::runtime_error("lseek error: node offset");
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
            if ((fnode.offToKeyEnd = lseek(fd, 0, SEEK_CUR)) == -1)
                throw std::runtime_error("lseek error: node offToKeyEnd");
            if (lseek(fd, h.offSize * n->count, SEEK_CUR) == -1) /* jump child array */
                throw std::runtime_error("lseek error: jump child array");
        }
        nodeMap.insert(pair<const BTNode<Key, T>* const, FNode>(n, fnode));
        if (n->parent != nullptr) {
            BTNode<Key, T>* p  = n->parent;
            auto            it = nodeMap.find(p);
            assert(it != nodeMap.end()); /* DEBUG */
            order_type<Key, T> pr = 0;
            while (p->child[pr] != n)
                pr++;
            if (pwrite(fd, &fnode.offset, h.offSize, it->second.offToKeyEnd + pr * h.offSize) == -1)
                throw std::runtime_error("pwrite error: child offset");
        }
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
        // TODO: add type here
    }

    template <class Key, class T> static void readHeader(const int& fd, BPlusTree<Key, T>*& btree, BptHeader& h) {
        assert(btree == nullptr); /* DEBUG */
        if (read(fd, &h, sizeof(BptHeader)) != sizeof(BptHeader))
            throw std::runtime_error("read error: header (part 1)");
        checkHeader<Key, T>(h);
        order_type<Key, T> order;
        if (read(fd, &order, h.orderSize) != h.orderSize)
            throw std::runtime_error("read error: order of BPlusTree");
        btree = new BPlusTree<Key, T>(order);  // throw if order < 3
        if (read(fd, &(btree->_size), h.sizeSize) != h.sizeSize)
            throw std::runtime_error("read error: size of BPlusTree");
        if (read(fd, &(btree->_node_count), h.sizeSize) != h.sizeSize)
            throw std::runtime_error("read error: node count of BPlusTree");
        if (read(fd, &(btree->_height), h.sizeSize) != h.sizeSize)
            throw std::runtime_error("read error: height of BPlusTree");
    }

    template <class Key, class T> static void checkHeader(const BptHeader& h) {
        if (h.magic != MAGIC)
            throw std::runtime_error("unknown file header");
        if (getType<Key>() == TP_UNKNOWN)
            throw std::runtime_error("unknown key type");
        else if (getType<Key>() != h.keyType)
            throw std::runtime_error("key type mismatch");
        if (getType<T>() == TP_UNKNOWN)
            throw std::runtime_error("unknown data type");
        else if (getType<T>() != h.dataType)
            throw std::runtime_error("data type mismatch");
        if (h.orderSize != (char)sizeof(order_type<Key, T>))
            throw std::runtime_error("unknown order_type of BPlusTree");
        if (h.sizeSize != (char)sizeof(size_type<Key, T>))
            throw std::runtime_error("unknown size_type of BPlusTree");
        if (h.offSize != (char)sizeof(off_t))
            throw std::runtime_error("unknown offset size");
    }

    template <class Key, class T>
    static BTNode<Key, T>* readNode(const int& fd, BTNode<Key, T>* p, const order_type<Key, T>& pr, ListPtr<Key, T>& h,
                                    const order_type<Key, T>& order) {
        char               type;
        order_type<Key, T> count;
        BTNode<Key, T>*    n = nullptr;
        if (read(fd, &type, 1) != 1)
            throw std::runtime_error("read error: node type");
        if (type == 0)
            n = new BTNode<Key, T>(order, false);
        else if (type == 1)
            n = new BTNode<Key, T>(order, true);
        else
            throw std::runtime_error("unknown node type");
        if (read(fd, &count, sizeof(order_type<Key, T>)) != sizeof(order_type<Key, T>))
            throw std::runtime_error("read error: key count of node");
        if ((p != nullptr && count < ((order + 1) / 2)) || count > order)
            throw std::runtime_error("key count of node breaks rule of BPlusTree");
        n->parent = p;
        if (p)
            p->child[pr] = n;
        if (n->type) {
            Key key;
            T   data;
            for (order_type<Key, T> i = 0; i < count; ++i) {
                readType(fd, key);
                readType(fd, data);
                n->insert(i - 1, key, &data);
            }
            BTNode<Key, T>* prior = BPlusTree<Key, T>::findPriorOne(n);
            if (prior) { /* link two leaf node */
                n->entry[0]->prior                   = prior->entry[prior->count - 1];
                prior->entry[prior->count - 1]->next = n->entry[0];
            }
            else
                h = n->entry[0]; /* update list head */
        }
        else {
            Key key;
            for (order_type<Key, T> i = 0; i < count; ++i) {
                readType(fd, key);
                n->insert(i - 1, key);
            }
            off_t offParent, offChild;
            if ((offParent = lseek(fd, 0, SEEK_CUR)) == -1)
                throw std::runtime_error("lseek error: record current offset");
            for (order_type<Key, T> i = 0; i < count; ++i) {
                if (lseek(fd, offParent + i * sizeof(off_t), SEEK_SET) == -1)
                    throw std::runtime_error("lseek error: move back to parent");
                if (read(fd, &offChild, sizeof(off_t)) != sizeof(off_t))
                    throw std::runtime_error("read error: child offset");
                if (lseek(fd, offChild, SEEK_SET) == -1)
                    throw std::runtime_error("lseek error: move to child");
                n->child[i] = readNode<Key, T>(fd, n, i, h, order);
            }
        }
        return n;
    }

    template <class T> static void readType(const int& fd, T& value) {
        if (typeid(T) == typeid(int)) {
            if (read(fd, &value, sizeof(int)) != sizeof(int))
                throw std::runtime_error("read error: int");
        }
        else if (typeid(T) == typeid(string)) {
            string::size_type strSize;
            if (read(fd, &strSize, sizeof(string::size_type)) != sizeof(string::size_type))
                throw std::runtime_error("read error: string size");
            char* cstr;
            if ((cstr = new char[strSize + 1]) == nullptr) /* allocate for \0 */
                throw std::runtime_error("can not allocate memory for string value");
            if (read(fd, cstr, strSize) != strSize)
                throw std::runtime_error("read error: char *");
            cstr[strSize] = 0;
            string* temp  = reinterpret_cast<string*>(&value);
            *temp         = string(cstr);
            delete[] cstr;
        }
        // TODO: add type here
    }
};
}  // namespace my