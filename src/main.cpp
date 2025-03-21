#include <iostream>
#include <fstream>
#include <print>
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "app.h"
#include "mesh3d.h"
#include "shaders.h"
#include "mesh.h"


// Global Application State
App gApp;
Mesh3D gMesh1;
Mesh3D gMesh2;
std::vector<Mesh3D*> meshPtrs{&gMesh1, &gMesh2};


/**
 * Create the graphics pipeline
 */
void CreateGraphicsPipeline() {
    std::string vertexShaderSource = LoadShaderAsString("../shaders/vert.glsl");
    std::string fragmentShaderSource = LoadShaderAsString("../shaders/frag.glsl");

    gApp.mGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void GetOpenGLVersionInfo() {
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
    std::cout << "Version: " << glGetString(GL_VERSION) << "\n";
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}


/**
 * Initialization of the graphics application. Typically, this will involve setting up
 * a window and the OpenGL Context (with the appropriate version)
 */
void InitializeProgram(App* app) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::println("{}", "SDL2 could not initialize video subsystem");
        return;
    }

    // Set up the OpenGL Context
    // Use OpenGL 4.1 core or greater -- Mac only supports up to 4.1
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // We want to request a double buffer for smooth updating
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create an application window using OpenGL that supports SDL
    app->mGraphicsAppWindow =
        SDL_CreateWindow("OpenGL", 0, 0, app->mScreenWidth, app->mScreenHeight, SDL_WINDOW_OPENGL);

    // Check if window did not create.
    if (app->mGraphicsAppWindow == nullptr) {
        std::println("{}", "SDL2 could not initialize window");
        return;
    }

    // Create an OpenGL Graphics Context
    app->mOpenGLContext = SDL_GL_CreateContext(gApp.mGraphicsAppWindow);

    // Check if context did not create
    if (app->mOpenGLContext == nullptr) {
        std::println("{}", "OpenGL could not initialize context");
    }

    // initialize the Glad Library / load all the relevant function pointers
    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::println("{}", "glad could not initialize");
    }
    // Display information from our above setup
    GetOpenGLVersionInfo();
}

/**
 * Function called int the main application loop to handle user input
 */
void Input(Mesh3D* mesh) {
    static int mouseX = gApp.mScreenWidth / 2;
    static int mouseY = gApp.mScreenHeight / 2;

    // Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event e;
    // Handle events in queue
    while (SDL_PollEvent(&e) != 0) {
        // If user posts an event to quit such as clicking the 'X' icon in the window corner
        if (e.type == SDL_QUIT) {
            std::println("{}", "Closing the application");
            gApp.mQuit = true;
        } else if (e.type == SDL_MOUSEMOTION) {
            mouseX += e.motion.xrel;
            mouseY += e.motion.yrel;
            gApp.mCamera.MouseLook(mouseX, mouseY);
        }
    }


    // Retrieve keyboard state
    const Uint8* state = SDL_GetKeyboardState(nullptr);

    if (state[SDL_SCANCODE_ESCAPE]) {
        gApp.mQuit = true;
    }
    if (state[SDL_SCANCODE_UP]) {
        mesh->mTransform.z += 0.0005f;
        // std::println("g_uOffset: {}", g_uOffset);
    }
    if (state[SDL_SCANCODE_DOWN]) {
        mesh->mTransform.z -= 0.0005f;
        // std::println("g_uOffset: {}", g_uOffset);
    }
    if (state[SDL_SCANCODE_LEFT]) {
        mesh->m_uRotate -= 0.05f;
        // std::println("g_uRotate: {}", g_uRotate);
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        mesh->m_uRotate += 0.05f;
        // std::println("g_uRotate: {}", g_uRotate);
    }

    float speed = 0.001f;
    if (state[SDL_SCANCODE_W]) {
        gApp.mCamera.MoveForward(speed);
    }
    if (state[SDL_SCANCODE_S]) {
        gApp.mCamera.MoveBackward(speed);
    }
    if (state[SDL_SCANCODE_A]) {
        gApp.mCamera.MoveLeft(speed);
    }
    if (state[SDL_SCANCODE_D]) {
        gApp.mCamera.MoveRight(speed);
    }
}

void MainLoop() {
    SDL_WarpMouseInWindow(gApp.mGraphicsAppWindow, gApp.mScreenWidth / 2, gApp.mScreenHeight / 2);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    // While the application is running
    while (!gApp.mQuit) {
        // Handle input
        Input(meshPtrs[0]);

        // set OpenGL state
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        glViewport(0, 0, gApp.mScreenWidth, gApp.mScreenHeight);
        glClearColor(1.f, 1.f, 0.f, 1.f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


        for (const auto meshPtr : meshPtrs) {
            MeshDraw(&gApp, meshPtr);
        }

        // Update the screen of the specified window
        SDL_GL_SwapWindow(gApp.mGraphicsAppWindow);
    }
}

/**
 * The last function of the program's execution. This destroys global objects
 * created in heap memory.
 */
void CleanUp() {
    SDL_DestroyWindow(gApp.mGraphicsAppWindow);
    gApp.mGraphicsAppWindow = nullptr;

    MeshDelete(&gMesh1);
    // Delete our graphics pipeline
    glDeleteProgram(gApp.mGraphicsPipelineShaderProgram);

    SDL_Quit();
}

int main() {
    // 1. Set up the graphics program
    InitializeProgram(&gApp);
    // Set up our camera
    const float aspect = (float)gApp.mScreenWidth / (float)gApp.mScreenHeight;
    gApp.mCamera.SetProjectionMatrix(glm::radians(45.0f),
                                     aspect,
                                     0.1f,
                                     10.0f
    );

    // 2. Setup our geometry
    MeshCreate(&gMesh1);
    gMesh1.mTransform.x = 0.0f;
    gMesh1.mTransform.y = 0.0f;
    gMesh1.mTransform.z = -2.0f;

    MeshCreate(&gMesh2);
    gMesh2.mTransform.x = 2.0f;
    gMesh2.mTransform.y = 0.1f;
    gMesh2.mTransform.z = -4.0f;

    // 3. Create our graphics pipel ine
    //   - At a minimum, this means the vertex and fragment shader
    CreateGraphicsPipeline();

    // 3.5 Attach a pipeline to each mesh
    MeshSetPipeline(&gMesh1, gApp.mGraphicsPipelineShaderProgram);
    MeshSetPipeline(&gMesh2, gApp.mGraphicsPipelineShaderProgram);

    // 4. Call the main application loop
    MainLoop();

    // 5. Call the cleanup function upon termination
    CleanUp();
    return 0;
}
