# スミス _sumisu_
Sumisu is the repo for Smith

## Setup
1. git clone git@github.com:hello/sumisu.git sumisu
2. git submodule update --init --recursive
3. ./config.sh ${APP} ${PLATFORM}
  * example _./config test nrf52_
4. run ./config.sh again to build, or run _make_ in build folder

## Toolchain
* CMake
* gcc-arm-non-eabi

## Architecture
A Smith project is composed of an **app** and  a **platform**, linked together by an **os** layer
* **app** is the device-agnostic, high level code which defines the business logic of the project.
* **platform** contains the platform specific drivers and implementation of the **os** layer.  
  In addition, a platform also implements the [**cmsis_rtos**](https://www.keil.com/pack/doc/CMSIS/RTOS/html/index.html) via a native OS.

  The following platforms are supported
    * nrf52832 + FreeRTOS
    * darwin + pthreads


* **os** is based on the cmsis_rtos API.  It provides hardware abstraction via services as well as all the OS specific functions.

## Folder Structure
