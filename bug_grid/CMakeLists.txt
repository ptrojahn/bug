cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(bug_grid)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(-g)

add_executable(bug_grid main.cpp)
target_link_libraries(bug_grid ${TORCH_LIBRARIES})
target_link_libraries(bug_grid sfml-graphics sfml-window sfml-system)
