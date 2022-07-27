request = function()
   headers = {}
   headers["Content-Type"] = "application/json"
   body = "{[{“src_app”: "foo", “dest_app”: "bar", "vpc_id": "vpc-0", "bytes_tx": 100, "bytes_rx": 300, "hour": 1},{“src_app”: "foo", “dest_app”: "bar", "vpc_id": "vpc-0", "bytes_tx": 200, "bytes_rx": 600, "hour": 1},{“src_app”: "baz", “dest_app”: "qux", "vpc_id": "vpc-0", "bytes_tx": 100, "bytes_rx": 500, "hour": 1},{“src_app”: "baz", “dest_app”: "qux", "vpc_id": "vpc-0", "bytes_tx": 100, "bytes_rx": 500, "hour": 2},{“src_app”: "baz", “dest_app”: "qux", "vpc_id": "vpc-1", "bytes_tx": 100, "bytes_rx": 500, "hour": 2}]}"
   return wrk.format("POST", "/v1/ivmero/api", headers, body)
end

response = function(status, headers, body)
   -- comment the following line to avoid server's excesive I/O to console on heavy load
   io.write("response: \n" .. body .. "\n---------------------------------------------\n")
end
