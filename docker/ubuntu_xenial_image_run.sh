#!/bin/sh

export DEBIAN_FRONTEND=noninteractive
echo "# delete all the apt list files to speed up 'apt-get update' command"
# delete all the apt list files to speed up 'apt-get update' command
rm -rf /var/lib/apt/lists/*

echo "# install apt repo for clang compiler"
# add apt repo for clang compiler
wget -O - http://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -
echo "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial main" >> /etc/apt/sources.list
echo "deb-src http://apt.llvm.org/xenial/ llvm-toolchain-xenial main" >> /etc/apt/sources.list

# add apt repo for apt-fast
add-apt-repository ppa:apt-fast/stable

apt-get update
apt-get install --no-install-recommends -y apt-fast
# install required packages
apt-fast -y install --no-install-recommends \
    libssl-dev autoconf libtool make automake zlib1g-dev
