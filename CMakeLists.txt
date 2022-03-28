cmake_minimum_required(VERSION 3.20.0)

project(Data-Structures VERSION 0.0.1)

file(GLOB SOURCES
    src/*.cpp
    src/*/*.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)