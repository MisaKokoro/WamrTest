# Copyright (C) 2022 Intel Corporation.  All rights reserved.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.0)
project(wasm-app)

set (WAMR_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

if (APPLE)
    set (HAVE_FLAG_SEARCH_PATHS_FIRST 0)
    set (CMAKE_C_LINK_FLAGS "")
    set (CMAKE_CXX_LINK_FLAGS "")
endif ()

set (CMAKE_SYSTEM_PROCESSOR wasm32)

if (NOT DEFINED WASI_SDK_DIR)
    set (WASI_SDK_DIR               "/opt/wasi-sdk")
endif ()

set (CMAKE_C_COMPILER_TARGET        "wasm32-wasi")
set (CMAKE_C_COMPILER               "${WASI_SDK_DIR}/bin/clang")
set (CMAKE_C_FLAGS                  "${CMAKE_C_FLAGS} -O3 -z stack-size=409600 -Wl,--initial-memory=2147483648")
set (CMAKE_C_FLAGS                  "${CMAKE_C_FLAGS}  -I/home/yanxiang/Desktop/WamrTest-probuf/include /home/yanxiang/Desktop/WamrTest-probuf/protobuf-wasm/lib/libprotobuf-c.a")
# 因为__assert_fail函数的原因，暂时不能nostdlib编译
# set (CMAKE_C_FLAGS                  "${CMAKE_C_FLAGS} -Wl,--strip-all,--no-entry -nostdlib")
set (CMAKE_C_FLAGS                  "${CMAKE_C_FLAGS} -Wl,--export=str_reverse")
set (CMAKE_C_FLAGS                  "${CMAKE_C_FLAGS} -Wl,--export=init_ringbuffer")
set (CMAKE_C_FLAGS                  "${CMAKE_C_FLAGS} -Wl,--export=add")
set (CMAKE_C_FLAGS                  "${CMAKE_C_FLAGS} -Wl,--allow-undefined")

add_executable(testapp.wasm testapp.c ../src/person.pb-c.c)
target_link_libraries(testapp.wasm)
