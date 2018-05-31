#!/bin/bash
#
# common entrypoint for the container

COMPILER_VERSION=${COMPILER_VERSION:+-$COMPILER_VERSION}

source "entrypoint.$DIST_NAME.sh"

# set and export the C and C++ compiler environment variables
export CXX=$CXX$COMPILER_VERSION
export CC=$CC$COMPILER_VERSION

# configure and compile the project
# run make targets specified in the environment variable MAKE_TARGET
mkdir -p build

cd build

if [[ ! -e Makefile ]]; then
    autoreconf -fi ..
    ../configure ${CONFIGURE_ARGS}
fi

make ${MAKE_TARGET//:/ }

ls -la
