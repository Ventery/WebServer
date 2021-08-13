#!/bin/sh

set -x

SOURCE_DIR=`pwd`
BUILD_DIR=${BUILD_DIR:-../build}
BUILD_TYPE=${BUILD_TYPE:-Debug}

mkdir -p $BUILD_DIR/$BUILD_TYPE"_WebServer" \
    && cd $BUILD_DIR/$BUILD_TYPE"_WebServer" \
    && cmake \
            -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
            $SOURCE_DIR \
    && make $* \
    && cp ./WebServer/WebServer /usr/local/bin/
