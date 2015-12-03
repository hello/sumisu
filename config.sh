#!/bin/bash
if [ "$#" -eq 0 ]; then
    echo "hello" &&
        cd build &&
        make
elif [ "$#" -ge 2 ]; then
    rm -rf build &&
        cmake -DTARGET_APP="$1" -DCMAKE_TOOLCHAIN_FILE="$2.cmake" . -Bbuild &&
        cd build &&
        make
fi
