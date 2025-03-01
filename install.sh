#!/bin/bash

# Install glaze if we don't have it already
GLAZE_DIRECTORY=/usr/local/include/glaze
if [ ! -d $GLAZE_DIRECTORY ]; then
  mkdir -p $GLAZE_DIRECTORY
  wget https://github.com/stephenberry/glaze/archive/refs/tags/v4.4.3.tar.gz
  tar -xf v4.4.3.tar.gz
  rm v4.4.3.tar.gz
  cp -r ./glaze-4.4.3/include/* $GLAZE_DIRECTORY
  rm -rf glaze-4.4.3
fi

if [ ! -d "./uWebSockets/" ]; then
  git clone --recurse-submodules git@github.com:uNetworking/uWebSockets.git
  cd uWebSockets
  make
  cd ..
fi

U_WEB_SOCKETS_DIRECTORY=/usr/local/include/uWebSockets/
U_SOCKETS_DIRECTORY=/usr/local/include/uSockets/
rm -rf $U_WEB_SOCKETS_DIRECTORY
rm -rf $U_SOCKETS_DIRECTORY
mkdir -p $U_WEB_SOCKETS_DIRECTORY
mkdir -p $U_SOCKETS_DIRECTORY

cp -r ./uWebSockets/src/* $U_WEB_SOCKETS_DIRECTORY
cp -r ./uWebSockets/uSockets/src/* $U_SOCKETS_DIRECTORY
