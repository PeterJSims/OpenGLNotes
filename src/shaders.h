//
// Created by Peter Sims on 3/21/25.
//

#ifndef SHADERS_H
#define SHADERS_H

#include <string>
#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>



// Shaders
// Here we set up two shaders, a vertex shader and a fragment shader.
// At a minimum, every Modern OpenGL program needs a vertex and a fragment shader.

// OpenGL provides functions that will compile the shader source code
// (which are simply stored as strings) at run-time.


inline std::string LoadShaderAsString(const std::string& fileName) {
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
inline GLuint CompileShader(GLuint type, const std::string& source) {
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
inline GLuint CreateShaderProgram(const std::string& vertexShaderSource,
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

#endif //SHADERS_H
