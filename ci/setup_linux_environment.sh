#!/bin/bash

set -e

sudo apt-get update 
sudo apt-get install --no-install-recommends --no-install-suggests -y \
         curl \
         ca-certificates \
         openjdk-8-jdk
sudo echo "deb [arch=amd64] http://storage.googleapis.com/bazel-apt stable jdk1.8" \
      | tee /etc/apt/sources.list.d/bazel.list
sudo curl https://bazel.build/bazel-release.pub.gpg | apt-key add -
sudo apt-get update
sudo apt-get install --no-install-recommends --no-install-suggests -y \
         bazel
sudo apt-get upgrade -y bazel
