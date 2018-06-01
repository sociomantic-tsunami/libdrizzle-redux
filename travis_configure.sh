#!/bin/bash
# Script for setting up OS specific CI test environment on travis-ci.org
#
# Copyright: Copyright (c) 2017 sociomantic labs GmbH. All rights reserved

set -e

# Script to print error message with the build is and environment variable
# MAKE_TARGET
#
# $1 error message to show
#
# Returns 0
print_error_msg ()
{
    test ! -z "$1" || exit 1
    echo "$1"
    echo "  os:         $TRAVIS_OS_NAME"
    echo "  lsb:        $DIST_NAME $DIST_VERSION"
    echo "  targets:    $MAKE_TARGET"

    return 0
}

# Script which is run before the installation script is called
#
# Only dependencies required in the native travis build environment should be
# installed in this step.
#
# For osx based builds homebrew is used to install required packages:
#   - sed, (libtool), openssl, mysql
# Returns 0
before_install()
{
    if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
        # jfrog dependency
        if [[ -n "$TRAVIS_TAG" && ! -e $HOME/bin/jfrog ]]; then
            mkdir -p $HOME/bin ;
            curl -XGET -L -k 'https://api.bintray.com/content/jfrog/jfrog-cli-go/$latest/jfrog-cli-linux-amd64/jfrog?bt_package=jfrog-cli-linux-amd64' > $HOME/bin/jfrog ;
            chmod a+x $HOME/bin/jfrog ;
        fi
    elif [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
        brew update
        brew install gnu-sed
        xcode_version=`xcodebuild -version | grep 'Xcode' | cut -f 2 -d ' '`
        # reinstall libtool on osx_image xcode6.x to ensure gsed is found by
        # glibtoolize
        if [[ "$xcode_version" == 6.* ]]; then
            brew reinstall libtool
        fi
        brew install openssl
        brew install mysql
    else
        print_error_msg "Invalid build configuration"
        return 1
    fi

    return 0
}

# Enable binary logging by setting log_bin and server_id in the [mysqld]
# section of the mysql config file
#
# $1 path to mysql config file
#
# Returns 0
enable_mysqlbinlog()
{
    echo "[mysqld]" | sudo tee -a "$1"
    echo "server-id=1" | sudo tee -a "$1"
    echo "log_bin=mysql_binlog" | sudo tee -a "$1"

    return 0
}

# Invoked by the travis hook before_script to prepare the build environment
#
# For linux based builds docker-compose is used to set up the build environment
#
# No dependencies required in the native travis build environment should be
# installed in this step. Use before_install instead
#
# Returns 0 or 1 if called with an invalid build configuration
before_script()
{
    if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
       # build images
        docker-compose -f ./docker-compose.yml \
            -f ./docker/docker-compose.$DIST_NAME.$DIST_VERSION.yml \
            build
    elif [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
        # Creates a MySQL config file with binary logging enabled and
        # starts the MySQL server
        enable_mysqlbinlog "$HOME/.my.cnf"

        mysql.server start
    else
        print_error_msg "Invalid build configuration"
        return 1
    fi

    return 0
}

# Invoked by the travis hook script.
#
# Runs ci tests on travis' native build environment or in docker containers
#
# Linux:
#   docker-compose is used to create a container with MySQL and a
#   container which configures, compiles, tests and possibly builds
#   distribution packages depending on the build configuration
#
# OSX:
#   Compiles and runs unittests in the travis osx build environment
#
# Returns 0 or 1 if called with an invalid build configuration
run_tests()
{
    if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
        docker-compose up --abort-on-container-exit
    elif [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
        if [ -d ./build ]; then
            rm -rf build
        fi
        mkdir build && cd build
        autoreconf -fi ..
        # Pass the root of the openssl installation directory
        ../configure --with-openssl=$(brew --prefix openssl)
        make check
    else
        print_error_msg "Invalid build configuration"
        return 1
    fi

    return 0
}

## Script entrypoint
if [[ $1 == "before_install" ]]; then
    before_install
elif [[ $1 == "before_script" ]]; then
    before_script
elif [[ $1 == "run_tests" ]]; then
    run_tests
else
    echo "invalid function call"
    exit 1
fi
