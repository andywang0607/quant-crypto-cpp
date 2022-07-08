FROM ubuntu:20.04

ENV DEBIAN_FRONTEND noninteractive

RUN apt update

RUN apt install build-essential -y
RUN apt install cmake -y
RUN apt install libssl-dev -y
RUN apt install wget -y
RUN apt install libcurl4-openssl-dev -y

RUN wget -c 'http://sourceforge.net/projects/boost/files/boost/1.77.0/boost_1_77_0.tar.bz2' && \
    tar xf boost_1_77_0.tar.bz2

WORKDIR /boost_1_77_0

RUN ./bootstrap.sh && \
    ./b2 && \
    ./b2 install

WORKDIR /

RUN printf "#!/bin/bash\n cd /\$1 && mkdir build && cd build\n cmake .. && make\n" >> build_project.sh
RUN chmod 755 build_project.sh