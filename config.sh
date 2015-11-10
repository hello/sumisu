
rm -rf build && cmake -DCMAKE_TOOLCHAIN_FILE=CMakeOSX.config . -Bbuild &&
    cd build && make
