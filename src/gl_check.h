//
// Created by Peter Sims on 3/21/25.
//

#ifndef GL_CHECK_H
#define GL_CHECK_H

#include <glad/glad.h>
#include <iostream>

static void GLClearAllErrors() {
    while (glGetError() != GL_NO_ERROR) {
    }
}

static bool GLCheckErrorStatus(const char* function, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "OpenGL Error: " << error
            << "\tLine: " << line
            << "\tFunction: " << function
            << '\n';
        return true;
    }

    return false;
}

#define GLCheck(x) GLClearAllErrors(); x; GLCheckErrorStatus(#x, __LINE__);

#endif //GL_CHECK_H
