#!/bin/bash

BUILD_DIR="./json-c-build"
LIB_DIR="./json-c"

echo "Building libs ..."

git clone https://github.com/json-c/json-c
mkdir -p "$BUILD_DIR"
pushd "$BUILD_DIR"
cmake "../$LIB_DIR" && make -j8
popd
ln -fs ./json-c-build/libjson-c.so  ./json-c-build/libjson-c.so.5
