-- MIT License
--
-- Copyright (c) 2016 ivmeroLabs.
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in all
-- copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
-- SOFTWARE.

wrk.method = "POST"
wrk.body = "[{\"src_app\": \"foo\", \"dest_app\": \"bar\", \"vpc_id\": \"vpc-0\", \"bytes_tx\": 100, \"bytes_rx\": 300, \"hour\": 1},{\"src_app\": \"foo\", \"dest_app\": \"bar\", \"vpc_id\": \"vpc-0\", \"bytes_tx\": 200, \"bytes_rx\": 600, \"hour\": 1},{\"src_app\": \"baz\", \"dest_app\": \"qux\", \"vpc_id\": \"vpc-0\", \"bytes_tx\": 100, \"bytes_rx\": 500, \"hour\": 1},{\"src_app\": \"baz\", \"dest_app\": \"qux\", \"vpc_id\": \"vpc-0\", \"bytes_tx\": 100, \"bytes_rx\": 500, \"hour\": 2},{\"src_app\": \"baz\", \"dest_app\": \"qux\", \"vpc_id\": \"vpc-1\", \"bytes_tx\": 100, \"bytes_rx\": 500, \"hour\": 2}]"
wrk.headers["Content-Type"] = "application/json"
