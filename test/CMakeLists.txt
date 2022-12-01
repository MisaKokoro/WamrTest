# Copyright (C) 2022 Intel Corporation.  All rights reserved.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.0)
project(test)

################ wasm application ###############
# -DWAMR_BUILD_BULK_MEMORY=1
# set(WAMR_BUILD_BULD_MEMORY 1) 
set (CMAKE_EXPORT_COMPILE_COMMANDS 1)
add_subdirectory(native)
add_subdirectory(native_with_ser)
add_subdirectory(wasm_with_ser)
add_subdirectory(wasm)