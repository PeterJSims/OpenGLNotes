#include <iostream>
#include <print>
#include <SDL2/SDL.h>
#include <glad/glad.h>

// Temp Globals
int gScreenWidth{640};
int gScreenHeight{480};
SDL_Window* gGraphicsAppWindow{nullptr};
SDL_GLContext gOpenGLContext{nullptr};

bool gQuit{false};

void GetOpenGLVersionInfo() {
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
    std::cout << "Version: " << glGetString(GL_VERSION) << "\n";
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

}

void InitializeProgram() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::println("{}", "SDL2 could not initialize video subsystem");
        return;
    }


    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);


    gGraphicsAppWindow = SDL_CreateWindow("OpenGL", 0, 0, gScreenWidth, gScreenHeight, SDL_WINDOW_OPENGL);
    if (gGraphicsAppWindow == nullptr) {
        std::println("{}", "SDL2 could not initialize window");
        return;
    }

    gOpenGLContext = SDL_GL_CreateContext(gGraphicsAppWindow);

    if (gOpenGLContext == nullptr) {
        std::println("{}", "OpenGL could not initialize context");
    }

    // initialize the Glad Library / load all the relevant function pointers
    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::println("{}", "glad could not initialize");
    }
    GetOpenGLVersionInfo();
}

void Input() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            std::println("{}", "Closing the application");
            gQuit = true;
        }
    }
}

void PreDraw(){}

void Draw(){}

void MainLoop() {
    while (!gQuit) {
        Input();

        PreDraw();

        Draw();

        // Update the screen
        SDL_GL_SwapWindow(gGraphicsAppWindow);
    }
}

void CleanUp() {
    SDL_DestroyWindow(gGraphicsAppWindow);
    SDL_Quit();
}

int main() {
    InitializeProgram();
    MainLoop();
    CleanUp();
    return 0;
}
