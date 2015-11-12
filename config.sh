
rm -rf build && cmake -DCMAKE_TOOLCHAIN_FILE=nrf52.cmake . -Bbuild &&
    cd build && make
