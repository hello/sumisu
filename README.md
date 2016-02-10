# スミス _sumisu_
Sumisu is the repo for Smith

## Setup
1. git clone git@github.com:hello/sumisu.git sumisu
2. git submodule update --init --recursive
3. for an initial build, configure the app and platform
```
./config.sh ${APP} ${PLATFORM}
```
  * example: ```./config test nrf52```
4. after that, just run ```./config.sh``` again to build the same setup, or run ```make``` in the **build** folder
5. all binaries and jlink flash scripts will be generated in the **bin** folder
  * to flash an application:
  ```
  ./tools/scripts/jlink_cli.sh < bin/${APPNAME}.jlink
  ```
  * to open up a RTT console:
  ```
  TODO: make this easier to invoke
  ./tools/scripts/jlink_cli.sh, in subsequent cli
  r
  g
  ./tools/jlink/JLinkRTTClient
  ```
  * Alternatively, if UART is used as the console:
  ```
  screen /dev/cu.USB${ID} ${BAUDRATE} -flow on
  ```

## Toolchain
* CMake
* gcc-arm-non-eabi
* Segger JLink

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
