#!/bin/sh
rm -rf build && cmake -DTARGET_APP="$1" -DCMAKE_TOOLCHAIN_FILE="$2.cmake" . -Bbuild &&
    cd build && make
