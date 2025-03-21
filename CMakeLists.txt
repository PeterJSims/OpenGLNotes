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
        ${LIBRARY_SOURCE_PATH}/glm/1.0.1/include
)
# Libraries
set(SDL2_LIBRARIES ${LIBRARY_SOURCE_PATH}/sdl2/2.30.10/lib/libSDL2.dylib)
set(GLEW_LIBRARIES ${LIBRARY_SOURCE_PATH}/glew/2.2.0_1/lib/libGLEW.dylib)
set(GLM_LIBRARIES ${LIBRARY_SOURCE_PATH}/glm/1.0.1/lib/libglm.dylib)
set(SOIL2_LIBRARIES ${LIBRARY_SOURCE_PATH}/SOIL2/lib/mac/libSOIL.a)
# Manually set the GLEW_LIBRARY (singular) to avoid "NOTFOUND"
set(GLEW_LIBRARY ${LIBRARY_SOURCE_PATH}/glew/2.2.0_1/lib/libGLEW.dylib)

find_package(SDL2 REQUIRED PATHS ${LIBRARY_SOURCE_PATH}/sdl2/2.30.10/lib/cmake/SDL2)
find_package(GLEW REQUIRED PATHS ${LIBRARY_SOURCE_PATH}/glew/2.2.0_1/lib/cmake/glew)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED)
file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR}/shaders)

add_executable(OpenGLTutorial src/main.cpp
        include/glad.c
        src/camera.h
        src/camera.cpp
        src/app.h
        src/mesh3d.h
        src/gl_check.h
        src/shaders.h
        src/mesh.cpp
        src/mesh.h
        src/transform.h
)


target_link_libraries(OpenGLTutorial
        ${SDL2_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${SOIL2_LIBRARIES}
        ${GLM_LIBRARIES}

        OpenGL::GL
        dl
        "-framework CoreFoundation"
)
