#!/bin/bash
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
pushd `dirname $0` > /dev/null
BASEDIR=`pwd`
popd > /dev/null

DOCKER_IMAGE="cmusei/falsa-mp-dev"
DOCKER_CONT="modelproblemdevcont"

if [ $# -gt 1 ]; then
    echo "usage: $0 [image_name]"
    exit -1
fi

if [ $# -gt 0 ]; then
    DOCKER_IMAGE=$1
fi


if [[ $OSTYPE == 'darwin'* ]]; then
  # macOS
  xhost +localhost
  # try to get the right display number from XQuartz, which is not always 0
  DISP=`ps aux | grep Xquartz | grep listen | grep -v grep | grep -v xinit | sed -n 's/.*:\([0-9]*\).*/\1/p'`
  if [ -z "$DISP" ]; then
    DISP="0"
  fi

  X_FWD="--add-host=host.docker.internal:host-gateway -e DISPLAY=host.docker.internal:$DISP"
  echo "Running on macOS"
else
  xhost +local:root
  X_FWD="-v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY -e LIBGL_ALWAYS_INDIRECT=0"
fi

docker run -it --rm --name $DOCKER_CONT \
       --mount type=bind,src="$BASEDIR",target="$BASEDIR" --workdir "$BASEDIR" \
       $X_FWD \
       --network=host \
       --platform=linux/x86_64 \
       --cap-add=SYS_PTRACE \
       --security-opt  \
       seccomp=unconfined \
       $DOCKER_IMAGE

