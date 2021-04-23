#!/bin/bash

set -e

ARCH=`arch`

if [[ "$ARCH" = "ppc64le" ]]; then
sudo apt install gnupg2 apt-transport-https -y
echo "deb https://oplab9.parqtec.unicamp.br/pub/repository/debian/ ./" | sudo tee /etc/apt/sources.list.d/bazel_ppc64le.list
curl --silent  https://oplab9.parqtec.unicamp.br/pub/key/openpower-gpgkey-public.asc  -o /tmp/openpower-gpgkey-public.asc
sudo apt-key add /tmp/openpower-gpgkey-public.asc
sudo rm -f /tmp/openpower-gpgkey-public.asc 
sudo apt update	

else

echo "deb [arch=amd64] http://storage.googleapis.com/bazel-apt stable jdk1.8" | sudo tee /etc/apt/sources.list.d/bazel.list
curl --silent https://bazel.build/bazel-release.pub.gpg | sudo apt-key add -
sudo apt-get update -o Dir::Etc::sourcelist="sources.list.d/bazel.list" \
-o Dir::Etc::sourceparts="-" \
-o APT::Get::List-Cleanup="0"

fi

sudo apt-get update
sudo apt-get install openjdk-8-jdk bazel -y
sudo apt-get install software-properties-common -y
sudo apt-get install cmake -y
