cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(bug)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(-g)

add_executable(bug main.cpp)
target_link_libraries(bug ${TORCH_LIBRARIES})
target_link_libraries(bug sfml-graphics sfml-window sfml-system)
