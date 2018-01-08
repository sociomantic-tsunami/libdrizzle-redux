#!/bin/bash
#
# entrypoint for the container

install_dependencies()
{
    if [[ "$DIST_NAME" == "ubuntu" ]]; then
        # install compiler
        COMPILER=$CXX
        if [[ ! -z $COMPILER_VERSION ]]; then
            COMPILER="$CXX-$COMPILER_VERSION"
        fi
        apt-fast install -y $COMPILER
    fi

    if [[ "$DIST_NAME" == "centos" ]]; then
        # install compiler
        if [[ "$CXX" == "clang" ]]; then
            yum install -y clang
        fi
    fi
}

# install dependencies
install_dependencies

# configure and compile the project
# run make targets specified in the environment variable MAKE_TARGET
mkdir -p build

cd build

if [[ ! -e Makefile ]]; then
    autoreconf -fi ..
    ../configure
fi

make ${MAKE_TARGET//:/ }
