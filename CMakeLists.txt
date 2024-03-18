cmake_minimum_required(VERSION 3.1.0)

project(Arene_light)

find_package(Threads)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)
add_compile_options(-O3 -flto -Wall -Wpedantic -ggdb)



aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST} objects/jeu.o objects/joueur_brutal_.o objects/joueur_random.o)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

