#!/bin/bash
#
# entrypoint for the container

# set and export the C and C++ compiler environment variables
export CXX=$CXX${COMPILER_VERSION:+-$COMPILER_VERSION}
export CC=$CC${COMPILER_VERSION:+-$COMPILER_VERSION}

install_dependencies()
{
    if [[ "$DIST_NAME" == "ubuntu" ]]; then
        # install compiler
        COMPILER_PACKAGE=$CXX
        if [[ "$CC" =~ "clang" ]]; then
            COMPILER_PACKAGE=$CC
        fi

        apt-fast install -y --no-install-recommends $COMPILER_PACKAGE
    fi

    if [[ "$DIST_NAME" == "centos" ]]; then
        # install compiler
        if [[ "$CC" =~ "clang" ]]; then
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
