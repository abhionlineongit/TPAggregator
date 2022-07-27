#ifndef _TRANSACTION_H_
#define _TRANSACTION_H_

#include <cstdint>
#include <functional>
#include <iostream>
#include "in_memory_db.hpp"

using namespace std;
using namespace web;

class Txn {
public:

	Txn(inMemoryDB *db) : local_db(db) {
		hour = -1;
		bytes_tx = bytes_rx = 0;
	};

	~Txn() {};

	bool setTransientValues(const string& key, const json::value &value) {
		if (key == "src_app") {
			return setSrcApp(value);
		} else if (key == "dest_app") {
			 return setDestApp(value);
		} else if (key == "vpc_id") {
            return setVpcId(value);
        } else if (key == "bytes_tx") {
             return setBytesTx(value);
        } else if (key == "bytes_rx") {
            return setBytesRx(value);
        } else if (key == "hour") {
             return setHour(value);
        } else {
			// ignore invalid key
			return true;
		}
	}

	void clearTransientValues() {
		hour = -1;
		bytes_rx = 0;
		bytes_tx = 0;
	}

	bool storeLocalDB() {
		if (hour >= 0) {
			if (!createDBKey()) {
                return false;
            }
			dbValue_t value;
			value.src_app = move(src_app);
			value.dest_app = move(dest_app);
			value.vpc_id = move(vpc_id);
			value.bytes_rx = bytes_rx;
			value.bytes_tx = bytes_tx;
			local_db->aggregate(hour, db_key, value);
            return true;
		}
		return false;
	}

    void getLocalDB(int hour, string &output) {
        local_db->get(hour, output);
    }

private:
    bool setSrcApp(const json::value &value) {
        if (value.is_string()) {
            src_app = value.as_string();
            return true;
        }
        return false;
    }

    bool setDestApp(const json::value &value) {
        if (value.is_string()) {
            dest_app = value.as_string();
            return true;
        }
        return false;
    }
    bool setVpcId(const json::value &value) {
        if (value.is_string()) {
            vpc_id = value.as_string();
            return true;
        }
        return false;
    }
    bool setBytesRx(const json::value &value) {
        if (value.is_integer()) {
            bytes_rx = value.as_integer();
            return true;
        }
        return false;
    }
    bool setBytesTx(const json::value &value) {
        if (value.is_integer()) {
            bytes_tx = value.as_integer();
            return true;
        }
        return false;
    }
    bool setHour(const json::value &value) {
        if (value.is_integer()) {
            hour = value.as_integer();
            return true;
        }
        return false;
    }

    bool createDBKey() {
		if (src_app.size() && dest_app.size() && vpc_id.size()) {
			int key_size = src_app.size() + dest_app.size() + vpc_id.size() + 2;
			db_key.reserve(key_size);
			db_key = src_app + ":" + dest_app + ":" + vpc_id;
			return true;
		}
		return false;
    }
	
	string src_app, dest_app, vpc_id, db_key;
	int bytes_tx, bytes_rx, hour;
	inMemoryDB *local_db;
};
#endif //_TRANSACTION_H_
