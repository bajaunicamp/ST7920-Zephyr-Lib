cmake_minimum_required(VERSION 3.22)

find_package(Zephyr)
project(template)

target_sources(app PRIVATE 
    src/main.c
    #src/outro_arquivo.c
)
target_include_directories(app PRIVATE inc)
