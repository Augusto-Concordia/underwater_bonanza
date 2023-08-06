cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

#================== STB_IMAGE =================

add_library(stb_image INTERFACE)
target_sources(stb_image INTERFACE stb_image/stb_image.h)
target_compile_definitions(stb_image INTERFACE STB_IMAGE_IMPLEMENTATION)
target_include_directories(stb_image INTERFACE stb_image)

#=================== GLM ===================

# includes GLM source CMake project
add_subdirectory(glm)

#=================== GLAD ===================

file(GLOB GLAD_SOURCES
        glad/include/KHR/khrplatform.h
        glad/include/glad/glad.h
        glad/src/glad.c)

add_library(glad STATIC ${GLAD_SOURCES})
target_include_directories(glad PUBLIC glad/include)

#=================== GLFW ===================

# finds glfw's OpenGL dependency from installed binaries
find_package(OpenGL REQUIRED)

# disables all glfw docs, tests and examples from builds
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# includes glfw source CMake project
add_subdirectory(glfw)