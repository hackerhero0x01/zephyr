# Copyright (c) 2021 Antmicro <www.antmicro.com>
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(devmem_load)

target_sources(app PRIVATE src/main.c)
