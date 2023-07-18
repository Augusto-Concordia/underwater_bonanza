cmake_minimum_required(VERSION 3.20)
project(underwater_bonanza)

set(CMAKE_CXX_STANDARD 20)

# finds our own OpenGL dependency from installed binaries
find_package(OpenGL REQUIRED)

# includes vendors' source CMake projects
add_subdirectory(vendor)

# add s_engine project and link glfw to it
file(GLOB_RECURSE UNDERWATER_BONANZA_FILES source/**.cpp)
add_executable(underwater_bonanza ${UNDERWATER_BONANZA_FILES})

target_include_directories(underwater_bonanza PRIVATE source)
target_link_libraries(underwater_bonanza PRIVATE glfw OpenGL::GL glm glad -static-libgcc -static-libstdc++)