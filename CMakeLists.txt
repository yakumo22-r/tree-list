# cmake version
cmake_minimum_required(VERSION 3.20)

# project info
project(
    sdl_study
    LANGUAGES CXX C
    DESCRIPTION "tree-list"
    VERSION 0.0.2
)

add_executable(sdl_study ./test.cpp ./tree_list.hpp)