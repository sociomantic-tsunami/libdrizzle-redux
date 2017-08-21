# Docker build file to create an image running centos7 with MySQL and build
# tools installed

FROM centos/mysql-56-centos7:latest
MAINTAINER andreas-bok-sociomantic <andreas.bok@sociomantic.com>

# Set environment variables
ENV MYSQL_USER=user \
    MYSQL_PASSWORD=pass \
    MYSQL_ROOT_PASSWORD=pass \
    MYSQL_SERVER=127.0.0.1 \
    MYSQL_PORT=3306 \
    MYSQL_DATABASE=test \
    MYSQL_MASTER_USER=user \
    MYSQL_MASTER_PASSWORD=pass \
    MYSQL_BINLOG_FORMAT=ROW \
    SKIP_TEST_ON_ERROR=0

# Change user to `root` and install required packages to build libdrizzle-redux.
USER root
RUN yum install -y deltarpm
RUN yum install -y rpm-build zlib-devel libtool openssl \
    openssl-devel automake autoconf gcc-c++

# Add host directory to container
ADD ./ /home/

WORKDIR /home

# Change back user to default user for the image: mysql
USER mysql

# Run the MySQL server with replication enabled
CMD [ "run-mysqld-master" ]
