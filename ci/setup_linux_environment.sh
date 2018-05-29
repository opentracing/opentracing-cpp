#!/bin/bash

set -e

apt-get update 
apt-get install --no-install-recommends --no-install-suggests -y \
         curl \
         ca-certificates \
         openjdk-8-jdk
echo "deb [arch=amd64] http://storage.googleapis.com/bazel-apt stable jdk1.8" \
      | tee /etc/apt/sources.list.d/bazel.list
curl https://bazel.build/bazel-release.pub.gpg | apt-key add -
apt-get update
apt-get install --no-install-recommends --no-install-suggests -y \
         bazel
apt-get upgrade -y bazel
