//
//  Created by Ivan Mejia on 12/24/16.
//
// MIT License
//
// Copyright (c) 2016 ivmeroLabs.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include <iostream>
#include <boost/core/demangle.hpp>
#include <typeinfo>
#include <usr_interrupt_handler.hpp>
#include <runtime_utils.hpp>
#include "microsvc_controller.hpp"
#include <redis++.h>
#include "redis_client.hpp"
#include <thread>
#include "in_memory_db.hpp"

redisClient *redis_client;
inMemoryDB *g_in_memory_db;
#define HASH_SIZE_DEFAULT 1031
#define DEFAULT_SERVICE_PORT 8080

void redis_thread(int enabled, int instance, int num_instances) {
    try {
        redis_client = new redisClient(enabled, instance, num_instances);
        redis_client->subscribe();
    } catch (exception &rte) {
        cout <<  rte.what() << "\n";
    }
}

int main(int argc, char *argv[]) {
    int opt;
    bool redis_enabled = false;
    int service_port = DEFAULT_SERVICE_PORT, num_redis_instances, redis_instance;

    static char const help[] =
        "-h                                 help (this message)\n"
        "-r                                 Enable Redis\n"
        "-i<int>                            Redis Instance ID\n"
        "-n<int>                            Number of Redis Instances\n"
        "-p<int>                            Service Port Number\n";

    while ((opt = getopt(argc, argv, "hri:p:n:")) != -1) {
        switch (opt) {
            case 'r': {
                redis_enabled = true;
                break;
            }
            case 'i': {
                redis_instance = (int)strtoul(optarg, NULL, 10);
                break;
            }
            case 'p': {
                service_port = (int)strtoul(optarg, NULL, 10);
                break;
            }
            case 'n': {
                num_redis_instances = (int)strtoul(optarg, NULL, 10);
                break;
            }
            case 'h':
            default: {
                cout << help;
                return -1;
            }
        }
    }
    InterruptHandler::hookSIGINT();
    MicroserviceController server;
    server.setEndpoint("http://host_auto_ip4:" + to_string(service_port) + "/flows");
    g_in_memory_db = new inMemoryDB(HASH_SIZE_DEFAULT);
    thread t(redis_thread, redis_enabled, redis_instance, num_redis_instances);
    t.detach();

    try {
        // wait for server initialization...
        server.accept().wait();
        std::cout << "TPAggregtoe service listening for requests at: " << server.endpoint() << '\n';
        
        InterruptHandler::waitForUserInterrupt();

        server.shutdown().wait();
    }
    catch(std::exception & e) {
        std::cerr << "somehitng wrong happen! :(" << e.what() << '\n';
    }
    catch(...) {
        RuntimeUtils::printStackTrace();
    }

    return 0;
}
