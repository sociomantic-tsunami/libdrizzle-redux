#!/bin/bash
# Script for setting up OS specific CI test environment on travis-ci.org
#
# Copyright: Copyright (c) 2017 sociomantic labs GmbH. All rights reserved

set -e

# Script to print error message with the build is and environment variable
# DIST_PACKAGE_TARGET
#
# $1 error message to show
#
# Returns 0
print_error_msg ()
{
    test ! -z "$1" || exit 1
    echo "$1"
    echo "  os:      $TRAVIS_OS_NAME"
    echo "  package: $DIST_PACKAGE_TARGET"

    return 0
}

# Script which is run before the installation script is called
#
# Installs dependencies
# linux:
#   deb:
#     - fpm
# osx:
#   - sed, (libtool), openssl, mysql
# Returns 0
before_install()
{
    if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
        if [[ "$DIST_PACKAGE_TARGET" == "DEB" ]]; then
            gem install fpm -v 1.8.1

            if [[ -n "$TRAVIS_TAG" && $TRAVIS_REPO_SLUG == "sociomantic-tsunami/libdrizzle-redux" ]]; then
                curl -XGET -L -k 'https://api.bintray.com/content/jfrog/jfrog-cli-go/$latest/jfrog-cli-linux-amd64/jfrog?bt_package=jfrog-cli-linux-amd64' > /tmp/jfrog ;
                chmod a+x /tmp/jfrog ;
                sudo cp /tmp/jfrog /usr/local/bin/jfrog ;
            fi
        fi
    elif [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
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
# Returns 0 or 1 if called with an invalid build configuration
before_script()
{
    if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
        if [[ "$DIST_PACKAGE_TARGET" == "DEB" ]]; then
            # Ensures MySQL is started with binlog enabled. This is done by
            # modifying the default my.cnf file.
            #  1. Enable replication in mysql config file
            #  2. Restart MySQL server
            #  3. Set root password to an empty string
            enable_mysqlbinlog "/etc/mysql/my.cnf"
            sudo service mysql restart
            mysql -u root -e "SET PASSWORD FOR 'root'@'localhost' = PASSWORD('')"
        elif [[ "$DIST_PACKAGE_TARGET" == "RPM" ]]; then
            # build a docker image with Centos 7 and MySQL
            docker build -t centos7_mysql56 -f docker/Dockerfile.centos.7 .
        else
            print_error_msg "Invalid build configuration"
            return 1
        fi
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
# - configures and compiles the project
# - runs all unittests with 'make check'
# - for builds on linux, a deb or rpm package is generated depending on the
#   current build configuration.
#
# Returns 0 or 1 if called with an invalid build configuration
run_tests()
{
    if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
        if [[ "$DIST_PACKAGE_TARGET" == "DEB" ]]; then
            autoreconf -fi
            ./configure
            make check
            make deb
        elif [[ "$DIST_PACKAGE_TARGET" == "RPM" ]]; then
            docker run -d --name db_container centos7_mysql56
            docker exec -u root db_container sh -c \
                "autoreconf -fi && ./configure && make rpm"
        else
            print_error_msg "Invalid build configuration"
            return 1
        fi
    elif [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
        autoreconf -fi
        # Pass the root of the openssl installation directory
        ./configure --with-openssl=$(brew --prefix openssl)
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
