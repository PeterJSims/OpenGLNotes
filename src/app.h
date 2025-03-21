//
// Created by Peter Sims on 3/21/25.
//

#ifndef APP_H
#define APP_H


#include <SDL2/SDL.h>
#include <glad/glad.h>
#include "camera.h"


struct App {
    int mScreenWidth{640};
    int mScreenHeight{480};
    bool mQuit{false};
    SDL_Window* mGraphicsAppWindow{nullptr};
    SDL_GLContext mOpenGLContext{nullptr};

    // shader
    // The following stores a unique ID for the graphics pipeline
    // program object that will be used for our OpenGL draw calls.
    GLuint mGraphicsPipelineShaderProgram{0};

    Camera mCamera;
};

#endif //APP_H
