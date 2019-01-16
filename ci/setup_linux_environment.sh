#!/bin/bash

set -e

echo "deb [arch=amd64] http://storage.googleapis.com/bazel-apt stable jdk1.8" | sudo tee /etc/apt/sources.list.d/bazel.list
curl --silent https://bazel.build/bazel-release.pub.gpg | sudo apt-key add -
sudo apt-get update -o Dir::Etc::sourcelist="sources.list.d/bazel.list" \
    -o Dir::Etc::sourceparts="-" \
    -o APT::Get::List-Cleanup="0"
sudo apt-get install openjdk-8-jdk bazel
sudo apt-get install software-properties-common

sudo add-apt-repository ppa:george-edison55/cmake-3.x -y
sudo apt-get update
sudo apt-get install cmake
