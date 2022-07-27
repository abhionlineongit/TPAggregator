cmake_minimum_required(VERSION 3.0)

project(TPAggregator)

# Configure required libraries ...

find_package(Boost REQUIRED COMPONENTS system thread log program_options chrono)
find_package(Threads REQUIRED)
find_package(OpenSSL 1.0.1 REQUIRED)
set(OPENSSL_VERSION "1.0.1")


message("-- configuring gcc options")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -DBOOST_LOG_DYN_LINK")

# source files ...
add_executable(${PROJECT_NAME} ./source/main.cpp
                               ./source/microsvc_controller.cpp
                               ./source/parser.cpp
                               ./source/foundation/network_utils.cpp
                               ./source/foundation/basic_controller.cpp)

# headers search paths ...
set(CPPRESTSDK_INCLUDE_DIR "/usr/include/cpprest")
set(MICROSERVICE_INCLUDE_DIR "./source/foundation/include")
set(REDIS_INCLUDE_DIR "./redis_interface/include")
set(HIREDIS_INCLUDE_DIR "/usr/local/include")
set(PROJECT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/source/include")
set(HEADER_SEARCH_PATHS ${CPPRESTSDK_INCLUDE_DIR} ${MICROSERVICE_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR} ${REDIS_INCLUDE_DIR} ${HIREDIS_INCLUDE_DIR} ${PROJECT_INCLUDE_DIR})

message(HEADER_SEARCH_PATHS" ${HEADER_SEARCH_PATHS}")

 # libraries search paths ...
set(OPENSSL_LIBS "${OPENSSL_LIBRARIES}")
set(CPPRESTSDK_LIBRARY "/usr/lib/x86_64-linux-gnu/libcpprest.so")
set(ZIP_LIBRARY "/usr/lib/x86_64-linux-gnu/libz.so")
set(REDIS_LIBRARY "${PROJECT_SOURCE_DIR}/redis_interface/libs/libredis++.so")
set(HIREDIS_LIBRARY "/usr/local/lib/libhiredis.so")
set(HIREDIS_SSL_LIBRARY "/usr/local/lib/libhiredis_ssl.so")
set(LIBRARIES_SEARCH_PATHS ${CPPRESTSDK_LIBRARY} ${OPENSSL_LIBS} ${Boost_LIBRARIES} ${ZIP_LIBRARY} ${REDIS_LIBRARY} ${HIREDIS_LIBRARY} ${HIREDIS_SSL_LIBRARY})

message(LIBRARIES_SEARCH_PATHS " ${LIBRARIES_SEARCH_PATHS}")

include_directories(${HEADER_SEARCH_PATHS})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES_SEARCH_PATHS})
