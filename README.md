# TPAggregator
Service which aggregates the throughput between micro-services.

HOW TO BUILD/INSTALL

Install following packages on Ubuntu

  sudo apt-get install cmake openssl libboost-all-dev zlib1g-dev
  
  sudo apt-get install libhiredis-dev // microservice use [redisplusplus](https://github.com/sewenew/redis-plus-plus), which uses hiredis.
  
  sudo apt-get install libcpprest-dev // microservice is built on cpprestsdk (https://github.com/microsoft/cpprestsdk/blob/master/README.md)
  
  redisplusplus libraries and include files are added to the git repo under redis_interface/.
  
CLONE THE GIT REPO

BUILD
  
  $ mkdir build; cd build; cmake ..; make -j 8
   
RUN THE SERVICE

./build/TPAggregator -h

-h                                 help (this message)

-r                                 Enable Redis

-i<int>                            Redis Instance ID

-n<int>                            Number of Redis Instances

-p<int>                            Service Port Number

To run the standalone instance -> ./build/TPAggregator 

To run multiple instances sync'ed over Redis -> ./run_service.sh $num_of_instances. multiple redis instances are tested on same physical machine, but to scale the micro-service, these instances should run on different machines.

Also run Redis service on the machine sudo service redis-server start
  
FUNCTIONAL TESTS

Simple POST/GET CURL requests at the endpoint and verify the outout.

curl -X POST "http://172.17.0.14:8080/flows" -H 'Content-Type: applic
ation/json' -d '[{"src_app": "foo", "dest_app": "bar", "vpc_id": "vpc-0", "bytes_tx": 100, "bytes_rx": 300, "hour": 1},{"src_app": "f
oo", "dest_app": "bar", "vpc_id": "vpc-0", "bytes_tx": 200, "bytes_rx": 600, "hour": 1},{"src_app": "baz", "dest_app": "qux", "vpc_id
": "vpc-0", "bytes_tx": 100, "bytes_rx": 500, "hour": 1},{"src_app": "baz", "dest_app": "qux", "vpc_id": "vpc-0", "bytes_tx": 100, "b
ytes_rx": 500, "hour": 2},{"src_app": "baz", "dest_app": "qux", "vpc_id": "vpc-1", "bytes_tx": 100, "bytes_rx": 500, "hour": 2}]'
  
< HTTP/1.1 200 OK
          
curl http://172.17.0.14:8080/flows?hour=1
          
[{"src_app": "foo", "dest_app": "bar", "vpc_id": "vpc-0", "bytes_tx": "300", "bytes_rx": "900", "hour": "1},{"src_app": "baz", "dest_app": "qux", "vpc_id": "vpc-0", "bytes_tx": "100", "bytes_rx": "500", "hour": "1}]
          
Running service on two instances listening on port 8080 and 8081.
          
curl http://172.17.0.14:8081/flows?hour=1
          
[{"src_app": "foo", "dest_app": "bar", "vpc_id": "vpc-0", "bytes_tx": "300", "bytes_rx": "900", "hour": "1},{"src_app": "baz", "dest_app": "qux", "vpc_id": "vpc-0", "bytes_tx": "100", "bytes_rx": "500", "hour": "1}]
          
PERFORMANCE
          
Performance scaled linearly at the start but started to saturate CPU to 400% usage on 8 core machine. This is mostly due to contention. The traffic which is running is repeated in nature and map to the same bucket in the table, hence more contention between threads. If the traffic is diverse, many src-app, dest-app and vpc-id, the performance should scale linearly with number of cores.
          
cd tests/wrk
          
wrk -c1 -t1 -d10s -s benchmark_microsvc_POST.lua http://172.17.0.14:8080/flows --latency
          
Running 10s test @ http://172.17.0.14:8080/flows
  1 threads and 1 connections
Requests/sec:   1920.54
Transfer/sec:     71.27KB
          
wrk -c2 -t2 -d10s -s benchmark_microsvc_POST.lua http://172.17.0.14:8080/flows --latency
          
Running 10s test @ http://172.17.0.14:8080/flows
  2 threads and 2 connections
Requests/sec:   3878.57
Transfer/sec:    143.93KB
          
wrk -c4 -t4 -d10s -s benchmark_microsvc_POST.lua http://172.17.0.14:8080/flows --latency
          
Running 10s test @ http://172.17.0.14:8080/flows
  4 threads and 4 connections
Requests/sec:   5145.43
Transfer/sec:    190.94KB
          
wrk -c8 -t8 -d10s -s benchmark_microsvc_POST.lua http://172.17.0.14:8080/flows --latency
  8 threads and 8 connections   
Requests/sec:   5342.86
Transfer/sec:    198.27KB
  

