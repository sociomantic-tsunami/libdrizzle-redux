#!/usr/bin/env bash
# Script for setting up OS specific CI test environment on travis-ci.prg
#
# Copyright (C) 2016 Andreas Bok Andersen <bok.chan@gmail.com>


# Check if a formula is installed and installs if it it is not the case
#
# $1 formula to install
#
# Returns 0
function install_brew_formula {
    brew list "$1" &> /dev/null
    ret=$?
    if [[ ret -ne 0 ]]; then
        brew install "$1"
    fi

    return 0
}

# Script which is run before installation script is called
#
# If the target platform is apple osx, mysql56 and gnu-sed are installed
#
# Returns 0
function before_install {
    if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
        if [[ $(brew tap | grep "homebrew/versions" -c ) -ne 1 ]]; then
            brew tap homebrew/versions
            brew update
        fi
        install_brew_formula gnu-sed
        install_brew_formula mysql56
        install_brew_formula openssl

        # Links the header files of the openssl
        if [ ! -e "/usr/local/include/openssl" ]; then
            echo "Making symbolic link for openssl header files"
            ln -sf "$(brew --prefix openssl)/include/openssl" /usr/local/include/openssl
        fi
    elif [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
        gem install fpm
    fi

    mysql -u root -e "SET PASSWORD FOR 'root'@'localhost' = PASSWORD('')"

    return 0
}

# Invoked by the travis hook before_script
#
# Ensures MySQL is started with binlog enabled. This is done by setting
# modifying the default my.cnf file.
function before_script {
    mysql_conf="/etc/mysql/my.cnf"
    if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
        mysql_conf="/etc/my.cnf"
        sudo cp "$(brew --prefix mysql56)/support-files/my-default.cnf" $mysql_conf
        sudo sed -i -e 's/^# *server.id.*$/server_id=1/g' $mysql_conf
        sudo sed -i -e 's/^# *log.bin.*$/log_bin=mysql-bin/g' $mysql_conf
        mysql.server stop
        mysql.server start
    elif [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
        sudo sed -i -e 's/^# *server.id.*$/server_id=1/g' $mysql_conf
        sudo sed -i -e 's/^# *log.bin.*$/log_bin=mysql-bin/g' $mysql_conf
        sudo service mysql restart
    fi
    mysqld --print-defaults
    return 0
}

# Invoked by the travis hook script.
#
# - configures and compiles the project
# - runs all unittests with 'make check'
function run_tests {
    if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
        ./bootstrap.sh -a
        ./configure
    elif [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
        autoreconf -fi
        ./bootstrap.sh -c
        make deb
    fi

    # run all unittests
    make check
}

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
