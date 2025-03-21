#include <iostream>
#include <fstream>
#include <print>
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

// Temp Globals
int gScreenWidth{640};
int gScreenHeight{480};
bool gQuit{false};

SDL_Window* gGraphicsAppWindow{nullptr};
SDL_GLContext gOpenGLContext{nullptr};

// shader
// The following stores a unique ID for the graphics pipeline
// program object that will be used for our OpenGL draw calls.
GLuint gGraphicsPipelineShaderProgram{0};

// OpenGL Objects
// Vertex Array Object (VAO)
// Vertex array objects encapsulate all the items needed to render an object.
// For example, we may have multiple vertex buffer objects (VBO) related to rendering
// one object. The VAO allows us to set up the OpenGL state to render that object using
// the correct layout and correct buffers with one call after being setup.
GLuint gVertexArrayObject{0};

// Vertex Buffer Object (VBO)
// Vertex Buffer Objects store information relating to vertices (e.g. positions, normals,
// textures)
// VBOs are our mechanism for arranging geometry on the GPU.
GLuint gVertexBufferObject{0};

// Index Buffer Object
// This is used to store the array of indices that we want to draw from
// when we do indexed drawing.
GLuint gIndexBufferObject{0};

// Global offsets for rotations/zoom
float g_uOffset{-2.0f};
float g_uRotate{0.0f};
float g_uScale{0.5f};

// Global camera
Camera gCamera;

// Error Handling Routines
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


// Shaders
// Here we set up two shaders, a vertex shader and a fragment shader.
// At a minimum, every Modern OpenGL program needs a vertex and a fragment shader.

// OpenGL provides functions that will compile the shader source code
// (which are simply stored as strings) at run-time.


std::string LoadShaderAsString(const std::string& fileName) {
    // Resulting shader program loaded as a single string
    std::string result = "";
    std::string line = "";
    std::ifstream myFile(fileName.c_str());

    if (myFile.is_open()) {
        while (std::getline(myFile, line)) {
            result += line + '\n';
        }
        myFile.close();
    }
    return result;
}

/**
 * CompileShader will compile any valid vertex, fragment, geometry, tesselation, or compute
 * shader.
 * e.g. Compile a vertex shader: CompileShader(GL_VERTEX_SHADER, vertexShaderSource;
 *      Compile a fragment shader: CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
 */
GLuint CompileShader(GLuint type, const std::string& source) {
    GLuint shaderObject{};

    // Based on the type passed in, we create a shader object specifically for that type.
    if (type == GL_VERTEX_SHADER) {
        shaderObject = glCreateShader(GL_VERTEX_SHADER);
    } else if (type == GL_FRAGMENT_SHADER) {
        shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    }

    auto src{source.c_str()};
    // The source of our shader
    glShaderSource(shaderObject, 1, &src, nullptr);
    // Now compile the shader
    glCompileShader(shaderObject);
    // Retrieve the result of our compilation
    int result;
    // Our goal with glGetShaderiv is to retrieve the compilation status
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
        char* errorMessages{new char[length]};
        glGetShaderInfoLog(shaderObject, length, &length, errorMessages);


        if (type == GL_VERTEX_SHADER) {
            std::println("{}", "ERROR: GL_VERTEX_SHADER compilation failed!");
        } else if (type == GL_FRAGMENT_SHADER) {
            std::println("{}", "ERROR: GL_FRAGMENT_SHADER compilation failed!");
        }

        // Log shader compilation error
        std::cerr << errorMessages << std::endl;

        // Reclaim our memory
        delete[] errorMessages;

        // Delete our broken shader
        glDeleteShader(shaderObject);

        return 0;
    }
    return shaderObject;
};

/**
 * Creates a graphics program object (i.e. graphics pipeline) with a Vertex Shader
 * and a Fragment Shader
 */
GLuint CreateShaderProgram(const std::string& vertexShaderSource,
                           const std::string& fragmentShaderSource) {
    // Create a new program object
    GLuint programObject{glCreateProgram()};

    // Compile our shaders
    const GLuint myVertexShader{CompileShader(GL_VERTEX_SHADER, vertexShaderSource)};
    const GLuint myFragmentShader{CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource)};

    // Link our two shader programs together
    // Consider this the equivalent of taking two .cpp files and linking them to one
    // executable file.
    glAttachShader(programObject, myVertexShader);
    glAttachShader(programObject, myFragmentShader);
    glLinkProgram(programObject);

    // Validate our program
    glValidateProgram(programObject);

    // Once our final program Object has been created, we can detach and
    // then delete our individual shaders.

    return programObject;
}

/**
 * Create the graphics pipeline
 */
void CreateGraphicsPipeline() {
    std::string vertexShaderSource = LoadShaderAsString("../shaders/vert.glsl");
    std::string fragmentShaderSource = LoadShaderAsString("../shaders/frag.glsl");

    gGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void GetOpenGLVersionInfo() {
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
    std::cout << "Version: " << glGetString(GL_VERSION) << "\n";
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

/**
 * Set up your geometry during the vertex specification step
 */
void VertexSpecification() {
    // Geometry Data
    // Here we are going to store x, y, and z position attributes within vertexPositions
    // for the data. For now, this information is just stored in the CPU, and we are going
    // to store this data on the GPU shortly, in a call to glBufferData which will store this
    // information into a vertex buffer object (VBO).
    // Vertices on the CPU
    const std::vector<GLfloat> vertexData{
        // 0 - Vertex
        -0.5f, -0.5f, 0.0f, // Left vertex position
        1.0f, 0.0f, 0.0f, // color
        // 1 - Vertex
        0.5f, -0.5f, 0.0f, // Right vertex position
        0.0f, 1.0f, 0.0f, // color
        // 2 - Vertex
        -0.5f, 0.5f, 0.0f, // Top left vertex position
        0.0f, 0.0f, 1.0f, // color
        // 3 - Vertex
        0.5f, 0.5f, 0.0f, // Top right vertex position
        0.0f, 0.0f, 1.0f, // color
    };


    // Vertex Array Object (VAO) Setup
    // Note: We can think of the VAO as a 'wrapper around' all the Vertex Buffer Objects,
    // in the sense that it encapsulates all VBO state that we are setting up.
    // Thus, it is also important that we glBindVertexArray (i.e. select the VAO we want
    // to use) before our vertex buffer object operations
    glGenVertexArrays(1, &gVertexArrayObject);
    // We bind (i.e. select) to the Vertex Array Object (VAO) that we want to work within.
    glBindVertexArray(gVertexArrayObject);

    // Vertex Buffer Object (VBO) creation
    // Create a new vertex buffer object
    // Note: We'll see this pattern of code often in OpenGL of creating and binding to a buffer.
    glGenBuffers(1, &gVertexBufferObject);
    // Next we will do glBindBuffer.
    // Bind is equivalent to 'selecting the active buffer object' that we want to work with
    // in OpenGL.
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    // Now, in our currently bound buffer, we populate the data from our
    // 'vertexPositions' (which is in the CPU), onto a buffer that will live on the GPU
    glBufferData(GL_ARRAY_BUFFER, // Kind of buffer we are working with
                 vertexData.size() * sizeof(GLfloat), // Size of data in bytes
                 vertexData.data(), // Raw * array of data
                 GL_STATIC_DRAW // How we intend to use the data
    );

    const std::vector<GLuint> indexBufferData{2, 0, 1, 3, 2, 1};
    // Set up the Index Buffer Object (IBO aka EBO)
    glGenBuffers(1, &gIndexBufferObject);

    // For our given Vertex Array Object, we need to tell Opengl 'how'
    // the information in our buffer will be used.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 gIndexBufferObject);
    // Populate our Index Buffer (shifting data to GPU)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indexBufferData.size() * sizeof(GLuint),
                 indexBufferData.data(),
                 GL_STATIC_DRAW
    );

    // For the specific attribute in our vertex specification, we use
    // 'glVertexAttribPointer' to figure out how we are going to move
    // through the data.
    glVertexAttribPointer(0, // Corresponds to the enabled glEnableVertexAttribArray
                          3, // The number of components (e.g. x,y,z = 3 components)
                          GL_FLOAT, // Type
                          GL_FALSE, // Is the data normalized
                          sizeof(GLfloat) * 6, // Stride
                          (void*)0 // Offset (pointer)
    );

    // Color information
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(GLfloat) * 6,
                          (GLvoid*)(sizeof(GLfloat) * 3)
    );

    // Unbind our currently bound Vertex Array object
    glBindVertexArray(0);
    // Disable any attributes we opened in our Vertex Attribute Array,
    // as we do not want to leave them open.
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}


/**
 * Initialization of the graphics application. Typically, this will involve setting up
 * a window and the OpenGL Context (with the appropriate version)
 */
void InitializeProgram() {
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
    gGraphicsAppWindow = SDL_CreateWindow("OpenGL", 0, 0, gScreenWidth, gScreenHeight, SDL_WINDOW_OPENGL);

    // Check if window did not create.
    if (gGraphicsAppWindow == nullptr) {
        std::println("{}", "SDL2 could not initialize window");
        return;
    }

    // Create an OpenGL Graphics Context
    gOpenGLContext = SDL_GL_CreateContext(gGraphicsAppWindow);

    // Check if context did not create
    if (gOpenGLContext == nullptr) {
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
void Input() {
    static int mouseX = gScreenWidth/2;
    static int mouseY = gScreenHeight /2;

    // Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event e;
    // Handle events in queue
    while (SDL_PollEvent(&e) != 0) {
        // If user posts an event to quit such as clicking the 'X' icon in the window corner
        if (e.type == SDL_QUIT) {
            std::println("{}", "Closing the application");
            gQuit = true;
        } else if (e.type == SDL_MOUSEMOTION) {
            mouseX += e.motion.xrel;
            mouseY += e.motion.yrel;
            gCamera.MouseLook(mouseX,mouseY);
        }
    }


    // Retrieve keyboard state
    const Uint8* state = SDL_GetKeyboardState(nullptr);

    if (state[SDL_SCANCODE_ESCAPE]) {
        gQuit = true;
    }
    if (state[SDL_SCANCODE_UP]) {
        g_uOffset += 0.0005f;
        // std::println("g_uOffset: {}", g_uOffset);
    }
    if (state[SDL_SCANCODE_DOWN]) {
        g_uOffset -= 0.0005f;
        // std::println("g_uOffset: {}", g_uOffset);
    }
    if (state[SDL_SCANCODE_LEFT]) {
        g_uRotate -= 0.05f;
        // std::println("g_uRotate: {}", g_uRotate);
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        g_uRotate += 0.05f;
        // std::println("g_uRotate: {}", g_uRotate);
    }

    float speed = 0.001f;
    if (state[SDL_SCANCODE_W]) {
        gCamera.MoveForward(speed);
    }
    if (state[SDL_SCANCODE_S]) {
        gCamera.MoveBackward(speed);
    }
    if (state[SDL_SCANCODE_A]) {
        gCamera.MoveLeft(speed);
    }
    if (state[SDL_SCANCODE_D]) {
        gCamera.MoveRight(speed);
    }
}

/**
 * Typically we will use this for setting up some sort of initial state
 * Note: some of the calls may take place at different stages (post-processing) of the pipeline.
 */
void PreDraw() {
    // set OpenGL state
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glViewport(0, 0, gScreenWidth, gScreenHeight);
    glClearColor(1.f, 1.f, 0.f, 1.f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(gGraphicsPipelineShaderProgram);

    // Model transformation by translating our object into world space
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0, g_uOffset));
    model = glm::rotate(model, glm::radians(g_uRotate), glm::vec3(0.0f, 1.0f, 0.0f));

    // Update the model matrix by applying a rotation after our translation
    model = glm::scale(model, glm::vec3(g_uScale, g_uScale, g_uScale));


    // Retrieve our location of our Model Matrix
    GLint u_ModelMatrixLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_ModelMatrix");

    if (u_ModelMatrixLocation >= 0) {
        glUniformMatrix4fv(u_ModelMatrixLocation, 1, false, &model[0][0]);
    } else {
        std::println("Could not find u_ModelMatrix");
        exit(EXIT_FAILURE);
    }

    // Camera work section
    glm::mat4 viewMatrix = gCamera.GetViewMatrix();
    GLint u_ViewMatrixLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_ViewMatrix");
    if (u_ViewMatrixLocation >= 0) {
        glUniformMatrix4fv(u_ViewMatrixLocation, 1, false, &viewMatrix[0][0]);
    } else {
        std::println("Could not find u_ViewMatrix");
        exit(EXIT_FAILURE);
    }


    // Projection matrix (in perspective)
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), (float)gScreenWidth / (float)gScreenHeight,
        0.1f,
        10.0f);

    // Retrieve our location of our Model Matrix
    GLint u_ProjectionLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_Projection");
    if (u_ProjectionLocation >= 0) {
        glUniformMatrix4fv(u_ProjectionLocation, 1, false, &projection[0][0]);
    } else {
        std::println("Could not find u_Projection");
        exit(EXIT_FAILURE);
    }
}

/**
 * The render function gets called once per loop.
 */
void Draw() {
    // Enable our attributes
    glBindVertexArray(gVertexArrayObject);

    // Select the vertex buffer object we want to enable
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

    // Render data
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Stop using our current graphics pipeline
    // Note: This is not necessary if we only have one graphics pipeline
    glUseProgram(0);
}

void MainLoop() {
    SDL_WarpMouseInWindow(gGraphicsAppWindow, gScreenWidth / 2, gScreenHeight / 2);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    // While the application is running
    while (!gQuit) {
        // Handle input
        Input();
        // Set up anything (i.e. OpenGL state) that needs to be placed before draw calls
        PreDraw();
        // Draw calls in OpenGL
        Draw();
        // Update the screen of the specified window
        SDL_GL_SwapWindow(gGraphicsAppWindow);
    }
}

/**
 * The last function of the program's execution. This destroys global objects
 * created in heap memory.
 */
void CleanUp() {
    SDL_DestroyWindow(gGraphicsAppWindow);
    SDL_Quit();
}

int main() {
    // 1. Set up the graphics program
    InitializeProgram();

    // 2. Setup our geometry
    VertexSpecification();

    // 3. Create our graphics pipeline
    //   - At a minimum, this means the vertex and fragment shader
    CreateGraphicsPipeline();

    // 4. Call the main application loop
    MainLoop();

    // 5. Call the cleanup function upon termination
    CleanUp();
    return 0;
}
