cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

#================== SFML =================

# disables all sfml docs, tests and examples from builds
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TESTS OFF CACHE BOOL "" FORCE)

# disables all sfml modules except audio
set(SFML_BUILD_AUDIO ON CACHE BOOL "" FORCE)
set(SFML_BUILD_GRAPHICS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_SYSTEM OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_WINDOW OFF CACHE BOOL "" FORCE)

add_subdirectory(sfml)

#================== STB_IMAGE =================

add_library(stb_image INTERFACE)
target_sources(stb_image INTERFACE stb_image/stb_image.h)
target_compile_definitions(stb_image INTERFACE STB_IMAGE_IMPLEMENTATION)
target_include_directories(stb_image INTERFACE stb_image)

#=================== ASSIMP ===================

# includes assimp source CMake project
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)

# only builds the following importers
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLB_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)

add_subdirectory(assimp)

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