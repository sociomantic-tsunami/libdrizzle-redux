# Docker build file to create a centos image for building rpm packages

FROM centos:7
MAINTAINER andreas-bok-sociomantic <andreas-bok@sociomantic.com>

RUN yum install -y rpm-build zlib-devel libtool openssl \
    openssl-devel automake autoconf gcc-c++ && \
    yum clean all && \
    rm -r -f /var/cache/*

WORKDIR home
