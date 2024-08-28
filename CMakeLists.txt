cmake_minimum_required(VERSION 3.22)

set(BOARD stm32_min_dev)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(template)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})

target_include_directories(app PRIVATE inc)
