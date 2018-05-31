#!/bin/bash
#
# entrypoint for container running ubuntu

install_dependencies()
{
    COMPILER_PACKAGE=$CXX$COMPILER_VERSION

    if [[ "$CC" =~ "clang" ]]; then
        COMPILER_PACKAGE=$CC$COMPILER_VERSION
    fi

    apt-fast install -y --no-install-recommends $COMPILER_PACKAGE
}

# install dependencies
install_dependencies
