#!/bin/sh

export DEBIAN_FRONTEND=noninteractive
echo "# delete all the apt list files to speed up 'apt-get update' command"
# delete all the apt list files to speed up 'apt-get update' command
rm -rf /var/lib/apt/lists/*

echo "# install apt repo for clang compiler"
# install apt repo for clang compiler
echo "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial main" >> /etc/apt/sources.list
echo "deb-src http://apt.llvm.org/xenial/ llvm-toolchain-xenial main" >> /etc/apt/sources.list
gpg --keyserver keyserver.ubuntu.com --recv AF4F7421

echo "deb http://ppa.launchpad.net/saiarcot895/myppa/ubuntu xenial main" >> /etc/apt/sources.list
apt-key adv --keyserver keyserver.ubuntu.com --recv-keys DC058F40
apt-get update

apt-get install --no-install-recommends -y apt-fast
# install required packages
apt-fast -y install --no-install-recommends \
    libssl-dev autoconf libtool make automake zlib1g-dev
