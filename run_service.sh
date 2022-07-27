#!/bin/bash

num_instances=$1
service_port=8080

for (( n=1; n<=$num_instances; n++ )) do
	instance_id=$n
	./build/TPAggregator -r -i $instance_id -n $num_instances -p $service_port &
	ret=$?
	if test $ret -eq 0
	then
		echo "TPAggregator (Instance: instance_id) listening on service port $service_port"
	else
		echo "TPAggregator (Instance: instance_id) failed to start"
	fi
    service_port=$(($service_port+$instance_id))
done

