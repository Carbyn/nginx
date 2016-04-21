#!/bin/bash
# script to test modules

NGINX_HOME=/home/carbyn/test/nginx
NGINX_SRC=/home/carbyn/test/nginx-source/nginx-1.9.14
MODULES_SRC=/home/carbyn/dev/nginx/modules

# the module to be tested
# module=ngx_http_shithole_module
# module=ngx_http_fuckyou_module
module=ngx_http_yak_module

if [ -f $NGINX_HOME/logs/nginx.pid ]; then
    echo "gonna to stop nginx"
    $NGINX_HOME/sbin/nginx -s quit
fi

cd $NGINX_SRC \
    && ./configure --prefix=$NGINX_HOME --add-module=$MODULES_SRC/$module \
    && make && make install \
    && cp $MODULES_SRC/${module}_nginx.conf $NGINX_HOME/conf/nginx.conf \
    && $NGINX_HOME/sbin/nginx
