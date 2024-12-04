#
# FALSA Model Problem
# 
# Copyright 2024 Carnegie Mellon University.
# 
# NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
# INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
# UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR IMPLIED, AS
# TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF FITNESS FOR PURPOSE
# OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF THE
# MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND
# WITH RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT INFRINGEMENT.
# 
# Licensed under a MIT (SEI)-style license, please see license.txt or contact
# permission@sei.cmu.edu for full terms.
# 
# [DISTRIBUTION STATEMENT A] This material has been approved for public
# release and unlimited distribution.  Please see Copyright notice for non-US
# Government use and distribution.
# 
# This Software includes and/or makes use of Third-Party Software each subject
# to its own license.
# 
# DM24-0251
#

FROM --platform=linux/x86_64 ubuntu:22.04
 
ARG MODEL_PROBLEM_DIR=/model_problem
 
RUN \
    # setup general environments
    mkdir -p ${MODEL_PROBLEM_DIR} && \
    cd ${MODEL_PROBLEM_DIR} && \
    apt-get -y update && export DEBIAN_FRONTEND=noninteractive && \
    apt-get -y upgrade

RUN \
    apt-get -y install --fix-broken vim git cmake build-essential autoconf libtool pkg-config

RUN \
    apt-get -y install --fix-broken \
      ca-certificates \
      lsb-release \
      sudo \
      wget \
      python3 pip \
      locales \
      xterm \
      tmux tmuxp && \
    sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen && \
    dpkg-reconfigure --frontend=noninteractive locales

# install Zscaler certs
COPY ./certs/*.crt /certs/
RUN if [ -n "$(ls -A /certs/*.crt)" ]; then \
      cp -rf /certs/*.crt /usr/local/share/ca-certificates/; \
      update-ca-certificates; \
    fi
 
# install additional requirements

# PX4
# this is needed to run on macOS
ENV LIBGL_ALWAYS_INDIRECT=1

# The patch is to solve an issue that happens when running in macOS
COPY px4_poll_fix.patch ${MODEL_PROBLEM_DIR}
RUN \
    apt autoclean && apt autoremove && \
    apt install -y mesa-utils libgl1 && \
    cd ${MODEL_PROBLEM_DIR} && \
    # setup PX4
    git clone --depth 1 --branch v1.14.3 https://github.com/PX4/PX4-Autopilot.git --recursive && \
    cd PX4-Autopilot && \
    git apply ${MODEL_PROBLEM_DIR}/px4_poll_fix.patch && \
    bash ./Tools/setup/ubuntu.sh --no-nuttx && \
    # fix version error
    pip uninstall em && pip install empy==3.3.4 && \
    make px4_sitl

# MAVSDK
RUN \
    cd ${MODEL_PROBLEM_DIR} && \
    git clone --depth 1 --branch v2.12.12 https://github.com/mavlink/MAVSDK.git && \
    cd MAVSDK && \
    git submodule update --init --recursive && \
    cmake -DCMAKE_BUILD_TYPE=Debug -Bbuild/default -H. && \
    cmake --build build/default -j8 --target install

# POCO
RUN \
    cd ${MODEL_PROBLEM_DIR} && \
    git clone --depth 1 --branch poco-1.13.3-release https://github.com/pocoproject/poco.git && \
    cd poco && \
    mkdir cmake-build && \
    cd cmake-build && \
    cmake .. && \
    cmake --build . --target install

# gRPC
RUN \
    cd ${MODEL_PROBLEM_DIR} && \
    git clone -b v1.60.0 --depth 1 https://github.com/grpc/grpc

RUN \
    cd ${MODEL_PROBLEM_DIR}/grpc && \
    git submodule update --init && \
    mkdir -p cmake/build && \
    cd cmake/build && \
    cmake -DgRPC_INSTALL=ON \
          -DgRPC_BUILD_TESTS=OFF \
          ../.. && \
    make -j 4 && \
    make install    

RUN ldconfig

