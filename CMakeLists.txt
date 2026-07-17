cmake_minimum_required(VERSION 3.10.2)
project(ultra-potrace LANGUAGES CXX)

# Core library (static) - vendor Potrace
add_library(potrace_core STATIC src/main/cpp/potrace/potrace_core.cpp)
target_include_directories(potrace_core PUBLIC src/main/cpp/potrace)

# Shared library - JNI wrapper
add_library(ultra-potrace SHARED src/main/cpp/potrace_wrapper.cpp)
target_link_libraries(ultra-potrace potrace_core log)

find_library(log-lib log)
target_link_libraries(ultra-potrace ${log-lib})
