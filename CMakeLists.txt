cmake_minimum_required(VERSION 3.20)
project(underwater_bonanza)

set(CMAKE_CXX_STANDARD 20)

IF (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
ENDIF()

# finds our own OpenGL dependency from installed binaries
find_package(OpenGL REQUIRED)

# includes vendors' source CMake projects
add_subdirectory(vendor)

file(GLOB_RECURSE UNDERWATER_BONANZA_FILES source/**.cpp)
add_executable(underwater_bonanza ${UNDERWATER_BONANZA_FILES})

target_include_directories(underwater_bonanza PRIVATE source)

IF (WIN32)
    target_link_libraries(underwater_bonanza PRIVATE glfw OpenGL::GL glm glad stb_image assimp sfml-audio -static-libgcc -static-libstdc++)
ELSE()
    target_link_libraries(underwater_bonanza PRIVATE glfw OpenGL::GL glm glad stb_image assimp sfml-audio)
ENDIF()

# ============= POST BUILD =============

# Copy assets to build directory
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# Copy shaders to build directory
file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})

#[[
# Copy irrklang dll, so & dylib to build directory (according to OS)
IF (WIN32)
    file(COPY vendor/irrklang/win32/irrKlang.dll DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY vendor/irrklang/win32/ikpMP3.dll DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY vendor/irrklang/win32/ikpFlac.dll DESTINATION ${CMAKE_BINARY_DIR})
ELSEIF (APPLE)
    file(COPY vendor/irrklang/macos/libirrklang.dylib DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY vendor/irrklang/macos/libikpMP3.dylib DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY vendor/irrklang/macos/libikpFlac.dylib DESTINATION ${CMAKE_BINARY_DIR})
ELSE()
    file(COPY vendor/irrklang/linux/libirrklang.so DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY vendor/irrklang/linux/libikpMP3.so DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY vendor/irrklang/linux/libikpFlac.so DESTINATION ${CMAKE_BINARY_DIR})
ENDIF()]]
