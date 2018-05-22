#!/bin/sh

cmake -DBOARD="STM32L476" -DCMAKE_TOOLCHAIN_FILE="cmake/toolchain-arm-none-eabi.cmake" $@ .
