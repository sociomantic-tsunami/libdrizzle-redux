#!/bin/bash
#
# entrypoint for container running ubuntu

install_dependencies()
{
    COMPILER_PACKAGE=$CXX$COMPILER_VERSION

    if [[ "$CC" =~ "clang" ]]; then
        COMPILER_PACKAGE=$CC$COMPILER_VERSION
    fi

    apt-get install -y --no-install-recommends $COMPILER_PACKAGE lcov
}

# install dependencies
install_dependencies
