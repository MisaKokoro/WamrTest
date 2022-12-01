# Copyright (C) 2022 Intel Corporation.  All rights reserved.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.0)
project(native)


set (CMAKE_C_COMPILER_TARGET        "x86_64")
set (CMAKE_C_COMPILER               "gcc")
#优化到底开不开呢？
set (CMAKE_C_FLAGS                  "${CMAKE_C_FLAGS} -O0")

add_executable(native main.c)

