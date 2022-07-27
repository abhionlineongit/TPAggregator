#ifndef _MY_LOCAL_DB_H_
#define _MY_LOCAL_DB_H_

#include <cstdint>
#include <functional>
#include <iostream>
#include <mutex>
#include <atomic>

using namespace std;

#define READ_LOCK(lock, message)                    \
    if (pthread_rwlock_rdlock(&lock) != 0) {        \
        throw runtime_error(message);               \
    }

#define WRITE_LOCK(lock, message)                   \
    if (pthread_rwlock_wrlock(&lock) != 0) {        \
        throw runtime_error(message);               \
    }

#define UNLOCK(lock, message)                       \
    if (pthread_rwlock_unlock(&lock) != 0) {        \
        throw runtime_error(message);               \
    }

#define INIT_LOCK(lock, message)                    \
    if (pthread_rwlock_init(&lock, NULL) != 0) {    \
        throw runtime_error(message);               \
    }

#define DESTROY_LOCK(lock, message)                 \
    if (pthread_rwlock_destroy(&lock) != 0) {       \
        cout << message << "\n";                    \
    }

typedef struct dbValue {
    string src_app;
    string dest_app;
    string vpc_id;
    unsigned long bytes_rx;
    unsigned long bytes_tx;
} dbValue_t;

class dbNode {
public:
    dbNode(string key_, dbValue_t value_) : key(key_), value(value_) {
        next = nullptr;
    }
    ~dbNode() {
        next = nullptr; 
    }
    void aggregate(const dbValue_t& value_) {
        value.bytes_rx += value_.bytes_rx;
        value.bytes_tx += value_.bytes_tx;
    }
    size_t valueSize() {
        return (value.src_app.size() + value.dest_app.size() + value.vpc_id.size() + 2*sizeof(unsigned long));
    } 
    dbNode *next;
    string key;
    dbValue_t value;
};

class dbBucket {
public:
    dbBucket()  {
        INIT_LOCK(rwlock, "bucket rwlock initialize error!");
        row_size = string("src_app").size() + string("dest_app").size() +
                        string("vpc_id").size() + string("bytes_tx").size() +
                        string("bytes_rx").size() + string("hour").size() + 24; // 24 extra padding for ""
    }
    ~dbBucket() {
        DESTROY_LOCK(rwlock, "bucket rwlock destroy error!");
    }

    void get(int hour, string &output) const {
        READ_LOCK(rwlock, "db bucket rwlock lock error!");
        dbNode *node = head;
        while (node != nullptr) {
            output += "{";
            output += "\"src_app\": \"" + node->value.src_app + "\", ";
            output += "\"dest_app\": \"" + node->value.dest_app + "\", ";
            output += "\"vpc_id\": \"" + node->value.vpc_id + "\", ";
            output += "\"bytes_tx\": \"" + to_string(node->value.bytes_tx) + "\", ";
            output += "\"bytes_rx\": \"" + to_string(node->value.bytes_rx) + "\", ";
            output += "\"hour\": \"" + to_string(hour);
            output += "},";
            node = node->next;
        }
        UNLOCK(rwlock, "db bucket rwlock unlock error!");
    }

    size_t aggregate(const string &key, const dbValue_t &value) {
        WRITE_LOCK(rwlock, "db bucket rwlock lock error!")
        dbNode *prev = nullptr, *node = head;
        size_t new_bytes = 0;
        while (node != nullptr && node->key != key) {
            prev = node;
            node = node->next;
        }

        if (node == nullptr) {
            dbNode *new_node = new dbNode(key, value);
            if (head == nullptr) {
                head = new_node;
            } else {
                prev->next = new_node;
            }
            new_bytes = new_node->valueSize() + row_size;
        }
        else {  
            node->aggregate(value);
        }
        UNLOCK(rwlock, "db bucket rwlock unlock error!")
        return new_bytes;
    }
private:
    dbNode *head = nullptr;
    mutable pthread_rwlock_t rwlock;
    size_t row_size;
};

class dbTable {
public:
    dbTable(size_t _size = 1031) : size(_size) {
        hash_table = new dbBucket[size];
        total_size = 0;
        num_bytes = 0;
    }

    ~dbTable() {
        delete[] hash_table;
    }

    void get(int hour, string &output) const {
        size_t bytes = num_bytes;
        output.reserve(bytes);
        output = "[";
        for (size_t i = 0; i < size; i++) {
            hash_table[i].get(hour, output);
        }
        if (output.size() > 1) {
            output.pop_back();
        }
        output += "]";
    }

    void aggregate(const string &key, const dbValue_t &value) {
        size_t hashValue = hash<string>{}(key) % size;
        size_t new_bytes = hash_table[hashValue].aggregate(key, value);
        if (new_bytes) {
            num_bytes += new_bytes;
            total_size++;
        }
    }

private:
    dbBucket *hash_table;
    const size_t size;
    atomic<size_t> total_size, num_bytes;
};

class inMemoryDB {
public:
    inMemoryDB(size_t size_): size(size_) {
        INIT_LOCK(rwlock, "inMemoryDB rwlock initialize error!");
    }

    ~inMemoryDB() {
        DESTROY_LOCK(rwlock, "inMemoryDB rwlock destroy error!");
    }
    void get(int hour, string &output) const {
        dbTable *table = nullptr;
        READ_LOCK(rwlock, "inMemoryDB rwlock lock error!");
        auto it = kv_store.find(hour);
        if (it != kv_store.end()) {
            table = it->second;
        }
        UNLOCK(rwlock, "inMemooryDB rwlock unlock error!") 
        if (!table) {
            output = "[]";
            return;
        }
        table->get(hour, output);
    }

    void aggregate(int hour, const string &key, const dbValue_t &value) {
        READ_LOCK(rwlock, "inMemoryDB rwlock lock error!");
        auto it = kv_store.find(hour);
        if (it == kv_store.end()) {
            UNLOCK(rwlock, "inMemoryDB rwlock unlock error!");
            WRITE_LOCK(rwlock, "inMemoryDB rwlock lock error!");
            dbTable* table = new dbTable(size);
            kv_store[hour] = table;
        }
        UNLOCK(rwlock, "inMemoryDB rwlock unlock error!");
        kv_store[hour]->aggregate(key, value);
    }
#if 0
    void sweepAndResize() {
        for (auto it = kv_store.begin(); it != kv_store.end(); it++) {
            dbTable *table = it->second;
            if (table->shouldResize()) {
                dbTable* new_table = new dbTable(size << 1);
                table->rebalance(new_table);
            }
            it->second = new_table;
        }
    }
#endif    
private:
    unordered_map<int, dbTable*> kv_store;
    mutable pthread_rwlock_t rwlock;
    size_t size;
};
#endif //_MY_LOCAL_DB_H_
