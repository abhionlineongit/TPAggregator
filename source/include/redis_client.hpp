#ifndef _REDIS_CLIENT_H_
#define _REDIS_CLIENT_H_

#include <redis++.h>
#include "parser.hpp"

using namespace sw::redis;
using namespace std;

class redisClient {
public:
    redisClient(int enabled, int id, int num) : is_enabled(enabled), instance_id(id), num_instances(num) {
        if (is_enabled) {
            channel_name = "redis_channel" + to_string(instance_id);
            ConnectionOptions opts1;
            opts1.host = "127.0.0.1";
            opts1.port = 6379;
            opts1.socket_timeout = std::chrono::milliseconds(100);
            redis = new Redis(opts1);
        }
    }

    static void handle_message(string channel, string message) {
        json::value json_body = json::value::parse(message);
        processRequestBody(json_body);
    }

    void subscribe() {
        if (is_enabled) {
            Subscriber subscriber = redis->subscriber();
            subscriber.on_message(handle_message);
            for (int i = 0; i < num_instances; i++) {
                if (i == instance_id) {
                    continue;
                }
                string channel =  "redis_channel" + to_string(i);
                subscriber.subscribe(channel.c_str());
            }

            while (true) {
                try {
                    subscriber.consume();
                } catch (const TimeoutError &e) {
                    continue;
                } catch (...) {
                    throw;
                }
            }
        }
    }
    void publish(json::value& body) {
        if (is_enabled) {
            string message = body.serialize();
            redis->publish(channel_name.c_str(), message);
        }
    }

private:
    int instance_id;
    int num_instances;
    bool is_enabled;
    string channel_name;
    Redis *redis;
};
#endif //_REDIS_CLIENT_H_
