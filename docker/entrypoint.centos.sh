#!/bin/bash
#
# entrypoint for the container running centos

install_dependencies()
{
    # install compiler
    COMPILER_PACKAGE=$CC${COMPILER_VERSION:+.0}

    # install compiler
    if [[ "$CC" =~ "clang" ]]; then
        yum install -y COMPILER_PACKAGE
        export PATH=/opt/llvm-$COMPILER_VERSION.0/bin:$PATH
    fi
}

# install dependencies
install_dependencies
