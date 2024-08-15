cmake_minimum_required(VERSION 3.22)

set(BOARD stm32_min_dev)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(template)

target_sources(app PRIVATE 
    src/main.c
    #src/outro_arquivo.c
)
target_include_directories(app PRIVATE inc)
