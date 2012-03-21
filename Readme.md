varnishpurged
=============

small c++ app to purge items (urls) from varnish quickly. uses a redis based queue.


Synopsis
--------

start varnishpurged:

    varnishpurged localhost:8080 localhost:6379


add a job:

    redis-cli SADD varnishpurged:queue my.website.com/fubar/23
  
  
varnishpurged will connect to `localhost:8080` and send this:

    PURGE /fubar/23 HTTP/1.1
    Host: localhost
    Accept: */*
    X-Host: my.website.com



Usage
-----

### Install

    git clone git@github.com:paulasmuth/varnishpurged.git
    ./autogen.sh && make
    sudo make install



### Start

    varnishpurged varnish_host:port redis_host:port

e.g.

    varnishpurged 10.0.0.1:8080 10.0.0.23:6379


### Add Jobs

purge-jobs (urls that should be purged) are added without the leading `http://` and without port. they should look like this:

    my.website.com/product/1234


add a job with redis-cli

    SADD varnishpurged:queue my.website.com/product/1234


add a job from a rails app:

    redis = Redis.new
    redis.sadd("varnishpurged:queue", "my,website.com/product/1234")



License
-------

Copyright (c) 2012 Paul Asmuth & Christian Parpart @ Dawanda GmbH


Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to use, copy and modify copies of the Software, subject 
to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

