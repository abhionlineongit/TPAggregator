request = function()
   headers = {}
   headers["Content-Type"] = "application/json"
   return wrk.format("GET", "/v1/ivmero/api/service/test", headers, "")
end

response = function(status, headers, body)
   -- comment the following line to avoid server's excesive I/O to console on heavy load
   -- io.write("response: \n" .. body .. "\n---------------------------------------------\n")
end
