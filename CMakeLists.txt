cmake_minimum_required(VERSION 3.30)
project(OpenGLTutorial)

set(CMAKE_CXX_STANDARD 23)

set(LIBRARY_SOURCE_PATH "/Users/petersims/CppLibraries")
# Include directories
include_directories(
        ${LIBRARY_SOURCE_PATH}/sdl2/2.30.10/include
        ${LIBRARY_SOURCE_PATH}/glew/2.2.0_1/include
        ${LIBRARY_SOURCE_PATH}/glad/include
        ${LIBRARY_SOURCE_PATH}/SOIL2/include
)
# Libraries
set(SDL2_LIBRARIES ${LIBRARY_SOURCE_PATH}/sdl2/2.30.10/lib/libSDL2.dylib)
set(GLEW_LIBRARIES ${LIBRARY_SOURCE_PATH}/glew/2.2.0_1/lib/libGLEW.dylib)
set(SOIL2_LIBRARIES ${LIBRARY_SOURCE_PATH}/SOIL2/lib/mac/libSOIL.a)
# Manually set the GLEW_LIBRARY to avoid "NOTFOUND"
set(GLEW_LIBRARY ${LIBRARY_SOURCE_PATH}/glew/2.2.0_1/lib/libGLEW.dylib)

find_package(SDL2 REQUIRED PATHS ${LIBRARY_SOURCE_PATH}/sdl2/2.30.10/lib/cmake/SDL2)
find_package(GLEW REQUIRED PATHS ${LIBRARY_SOURCE_PATH}/glew/2.2.0_1/lib/cmake/glew)
find_package(OpenGL REQUIRED)

add_executable(OpenGLTutorial main.cpp
        include/glad.c)


target_link_libraries(OpenGLTutorial
        ${SDL2_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${SOIL2_LIBRARIES}

        OpenGL::GL
        dl
        "-framework CoreFoundation"
)
