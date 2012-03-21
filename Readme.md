varnishpurged
=============

small c++ app to purge urls from varnish quickly (using a redis-based queue) 


Installation
------------

    git clone git@github.com:paulasmuth/varnishpurged.git
    ./autogen.sh && make
    sudo make install



Usage
-------

    varnishpurged varnish_host:port redis_host:port

e.g.

    varnishpurged 10.0.0.1:8080 10.0.0.23:6379


Adding Purge-Jobs
-----------------

purge-jbos (urls) are added without the leading http:// and without port! they should look like this:

    de.dawanda.com/product/1234


add a job with redis-cli

    SADD varnishpurged:queue de.dawanda.com/product/1234


add a job from a rails app:

    redis = Redis.new
    redis.sadd("varnishpurged:queue", "de.dawanda.com/product/1234")



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

