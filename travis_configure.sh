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
# Installs fpm on needed for deb packaging
#
# Other build configurations are not handled in this function
#
# Returns 0
before_install()
{
    if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
        if [[ "$DIST_PACKAGE_TARGET" == "DEB" ]]; then
            gem install fpm
        fi
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
    sudo sed -i -e 's/^# *server.id.*$/server_id=1/g' $1
    sudo sed -i -e 's/^# *log.bin.*$/log_bin=mysql-bin/g' $1

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
            docker build -t centos7_mysql56 -f docker/Dockerfile .
        else
            print_error_msg "Invalid build configuration"
            return 1
        fi

    fi

    return 0
}

# Invoked by the travis hook script.
#
# - configures and compiles the project
# - runs all unittests with 'make check'
# - generates deb or rpm package depending on the current build configuration
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
