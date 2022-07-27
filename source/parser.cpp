#include <std_micro_service.hpp>
#include "microsvc_controller.hpp"
#include "transaction.hpp"
#include "redis_client.hpp"

extern redisClient *redis_client;
extern inMemoryDB *g_in_memory_db;

void processRequestBody(json::value &json_body) {
    if (!json_body.is_array()) {
        throw runtime_error("Invalid JSON: should be a table");
    }
    Txn tx(g_in_memory_db);
    auto json_array = json_body.as_array();
    for (int i = 0; i < json_array.size(); i++)
    {
        auto json_obj = json_array[i];
        if (!json_obj.is_object()) {
            throw runtime_error("Invalid JSON: Object type doesn't match");
        }
        auto json_object = json_obj.as_object();
        for (auto iter = json_object.cbegin(); iter != json_object.cend(); ++iter)
        {
            auto &key = iter->first;
            auto &value = iter->second;
            if (!tx.setTransientValues(key, value)) {
                throw runtime_error("Invalid JSON: Key/Value type is not valid");
            }
        }
        if (!tx.storeLocalDB()) {
            throw runtime_error("Error while storing into local DB");
        }
        tx.clearTransientValues();
    }
    redis_client->publish(json_body);
}
