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

#include <std_micro_service.hpp>
#include "microsvc_controller.hpp"
#include "transaction.hpp"
#include "parser.hpp"
#include "redis_client.hpp"

using namespace web;
using namespace http;
extern inMemoryDB *g_in_memory_db;

void MicroserviceController::initRestOpHandlers() {
    _listener.support(methods::GET, std::bind(&MicroserviceController::handleGet, this, std::placeholders::_1));
    _listener.support(methods::PUT, std::bind(&MicroserviceController::handlePut, this, std::placeholders::_1));
    _listener.support(methods::POST, std::bind(&MicroserviceController::handlePost, this, std::placeholders::_1));
    _listener.support(methods::DEL, std::bind(&MicroserviceController::handleDelete, this, std::placeholders::_1));
    _listener.support(methods::PATCH, std::bind(&MicroserviceController::handlePatch, this, std::placeholders::_1));
}

void MicroserviceController::handleGet(http_request message) {
    try {
        auto query_params = requestQueryParams(message);
        auto it = query_params.find("hour");
        if (it != query_params.end()) {
            string output;
            int hour = stoi(it->second);
            g_in_memory_db->get(hour, output);
            message.reply (status_codes::OK, U(output), "content_type=application/json; charset=utf-8");
        } else {
            message.reply(status_codes::BadRequest);
        }
    } catch (exception &rte) {
        std::cout <<  rte.what() << "\n";
        if (string(rte.what()).find("Invalid JSON") != string::npos) {
            message.reply(status_codes::BadRequest);
        } else {
            message.reply(status_codes::InternalError);
        }
    }
}

void MicroserviceController::handlePost(http_request message) {
    try {
        auto json_body = message.extract_json().get();
        processRequestBody(json_body);
        message.reply(status_codes::OK);
    } catch (exception &rte) {
        std::cout <<  rte.what() << "\n";
        message.reply(status_codes::InternalError);
    }
}

void MicroserviceController::handlePatch(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::PATCH));
}

void MicroserviceController::handlePut(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::PUT));
}

void MicroserviceController::handleDelete(http_request message) {    
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::DEL));
}

void MicroserviceController::handleHead(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::HEAD));
}

void MicroserviceController::handleOptions(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::OPTIONS));
}

void MicroserviceController::handleTrace(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::TRCE));
}

void MicroserviceController::handleConnect(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::CONNECT));
}

void MicroserviceController::handleMerge(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::MERGE));
}

json::value MicroserviceController::responseNotImpl(const http::method & method) {
    auto response = json::value::object();
    response["serviceName"] = json::value::string("C++ Mircroservice Sample");
    response["http_method"] = json::value::string(method);
    return response ;
}
