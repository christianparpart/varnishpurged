varnishpurged
=============

small deamon to purge urls from varnish efficcently


adding purge-jobs/urls
----------------------

urls are added without the leading http:// and without port! they should look like this:

    de.dawanda.com/product/1234


add a job with redis-cli

    SADD varnishpurged:queue de.dawanda.com/product/1234


add a job from the rails app:

    REDIS.sadd("varnishpurged:queue", "de.dawanda.com/product/1234")




running varnishpurged
---------------------

    (here be dragons)



License
-------

Copyright (c) 2012 Christian Parpart & Paul Asmuth / Dawanda GmbH

MIT LICENSE HERE???