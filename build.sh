#!/bin/bash

if [[ "$(uname)" == "Darwin" ]]; then
  echo "----- build.sh script for MacOS -----"
  cmake --build --preset=debug-macos
  ./build/macos/Debug/Dying\ Anima
else
  echo "----- build.sh script for Linux -----"
  cmake --build --preset=debug-linux
  ./build/linux/Debug/Dying\ Anima
fi
