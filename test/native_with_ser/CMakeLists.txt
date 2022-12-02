# Copyright (C) 2022 Intel Corporation.  All rights reserved.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.0)
project(native_with_ser)

set (CMAKE_C_COMPILER_TARGET        "x86_64")
set (CMAKE_C_COMPILER               "gcc")
set (CMAKE_C_FLAGS                  "${CMAKE_C_FLAGS} -O0")
set (CMAKE_C_FLAGS                  "${CMAKE_C_FLAGS}  -I/home/yanxiang/Desktop/WamrTest-probuf/include")


add_executable(native_with_ser main.c ../../src/struct.pb-c.c)
target_link_libraries(native_with_ser -lprotobuf-c)
