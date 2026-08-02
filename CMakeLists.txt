cmake_minimum_required(VERSION 3.10)
project(PhysicsEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
include_directories(src)
add_executable(PhysicsSim
    src/main.cpp
    src/Physics/PointMass.cpp
    src/Physics/PhysicsWorld.cpp
    src/glad.c
)

target_link_libraries(PhysicsSim PRIVATE glfw glad::glad glm::glm)
