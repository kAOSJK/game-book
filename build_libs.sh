#!/bin/bash

BUILD_DIR="./json-c-build"
LIB_DIR="./json-c"

echo "Building libs ..."

git clone https://github.com/json-c/json-c
mkdir -p "$BUILD_DIR"
pushd "$BUILD_DIR"
cmake "../$LIB_DIR" && make -j8
popd

# NO LDD ERRORS
ln -fs ./json-c-build/libjson-c.so  ./json-c-build/libjson-c.so.5
# NO C89 ERRORS
sed -i '314s/.*//' ./json-c/linkhash.h
sed -i '314s/.*/#if (defined(AIX_CC) || (defined(_MSC_VER) \&\& (_MSC_VER <= 1800)) || (defined(__STRICT_ANSI__)))/' ./json-c/linkhash.h
