cmake_minimum_required(VERSION 3.10)
project(PhysicsEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
include_directories(src)
file(GLOB_RECURSE SOURCES
    "src/*.c"
)
add_executable(PhysicsSim ${SOURCES})

target_link_libraries(PhysicsSim PRIVATE glfw glad::glad glm::glm)
