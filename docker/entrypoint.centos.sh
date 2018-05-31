#!/bin/bash
#
# entrypoint for the container running centos

install_yum_repo()
{
    if ! type "$CC" &> /dev/null; then
        if [[ "$CC" =~ "clang" && -n "$COMPILER_VERSION" ]]; then
        echo "\
[alonid-llvm-$COMPILER_VERSION.0]
name=Copr repo for llvm-$COMPILER_VERSION.0 owned by alonid
baseurl=https://copr-be.cloud.fedoraproject.org/results/alonid/llvm-$COMPILER_VERSION.0/epel-7-\$basearch/
type=rpm-md
skip_if_unavailable=True
gpgcheck=1
gpgkey=https://copr-be.cloud.fedoraproject.org/results/alonid/llvm-$COMPILER_VERSION.0/pubkey.gpg
repo_gpgcheck=0
enabled=1
enabled_metadata=1
" >> /etc/yum.repos.d/epel.repo
        fi
    fi
}

install_dependencies()
{
    # install compiler
    COMPILER_PACKAGE=$CC${COMPILER_VERSION:+.0}

    # install compiler
    if [[ "$CC" =~ "clang" ]]; then
        install_yum_repo
        yum install -y COMPILER_PACKAGE
        export PATH=/opt/llvm-$COMPILER_VERSION.0/bin:$PATH
    fi
}

# install dependencies
install_dependencies


