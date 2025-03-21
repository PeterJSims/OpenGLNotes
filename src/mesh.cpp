//
// Created by Peter Sims on 3/21/25.
//

#include "mesh.h"

#include <iostream>
#include <print>
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "app.h"
#include "camera.h"
#include "mesh3d.h"

/**
 * Attach a graphics pipeline to the mesh
 */

void MeshSetPipeline(Mesh3D* mesh, GLuint pipeline) {
    mesh->mPipeline = pipeline;
}



void MeshCreate(Mesh3D* mesh) {
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
    glGenVertexArrays(1, &mesh->mVertexArrayObject);
    // We bind (i.e. select) to the Vertex Array Object (VAO) that we want to work within.
    glBindVertexArray(mesh->mVertexArrayObject);

    // Vertex Buffer Object (VBO) creation
    // Create a new vertex buffer object
    // Note: We'll see this pattern of code often in OpenGL of creating and binding to a buffer.
    glGenBuffers(1, &mesh->mVertexBufferObject);
    // Next we will do glBindBuffer.
    // Bind is equivalent to 'selecting the active buffer object' that we want to work with
    // in OpenGL.
    glBindBuffer(GL_ARRAY_BUFFER, mesh->mVertexBufferObject);
    // Now, in our currently bound buffer, we populate the data from our
    // 'vertexPositions' (which is in the CPU), onto a buffer that will live on the GPU
    glBufferData(GL_ARRAY_BUFFER, // Kind of buffer we are working with
                 vertexData.size() * sizeof(GLfloat), // Size of data in bytes
                 vertexData.data(), // Raw * array of data
                 GL_STATIC_DRAW // How we intend to use the data
    );

    const std::vector<GLuint> indexBufferData{2, 0, 1, 3, 2, 1};
    // Set up the Index Buffer Object (IBO aka EBO)
    glGenBuffers(1, &mesh->mIndexBufferObject);

    // For our given Vertex Array Object, we need to tell Opengl 'how'
    // the information in our buffer will be used.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 mesh->mIndexBufferObject);
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
 * The render function gets called once per loop.
 */
void MeshDraw(const Mesh3D* mesh) {
    if (mesh == nullptr) { return; }

    // Set which graphics pipeline to use
    glUseProgram(mesh->mPipeline);
    // Enable our attributes
    glBindVertexArray(mesh->mVertexArrayObject);

    // Select the vertex buffer object we want to enable
    glBindBuffer(GL_ARRAY_BUFFER, mesh->mVertexBufferObject);

    // Render data
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Stop using our current graphics pipeline
    // Note: This is not necessary if we only have one graphics pipeline
    glUseProgram(0);
}


/**
 * Updating/manipulating the state of a Mesh3D object
 * Note: some of the calls may take place at different stages (post-processing) of the pipeline.
 */
void MeshUpdate(const Mesh3D* mesh,const App& app) {
    glUseProgram(mesh->mPipeline);

    // Model transformation by translating our object into world space
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(mesh->mTransform.x, mesh->mTransform.y, mesh->mTransform.z));
    model = glm::rotate(model, glm::radians(mesh->m_uRotate), glm::vec3(0.0f, 1.0f, 0.0f));
    // Update the model matrix by applying a rotation after our translation
    model = glm::scale(model, glm::vec3(mesh->m_uScale, mesh->m_uScale, mesh->m_uScale));


    // Retrieve our location of our Model Matrix
    GLint u_ModelMatrixLocation = glGetUniformLocation(app.mGraphicsPipelineShaderProgram, "u_ModelMatrix");

    if (u_ModelMatrixLocation >= 0) {
        glUniformMatrix4fv(u_ModelMatrixLocation, 1, false, &model[0][0]);
    } else {
        std::println("Could not find u_ModelMatrix");
        exit(EXIT_FAILURE);
    }

    // Camera work section
    glm::mat4 viewMatrix = app.mCamera.GetViewMatrix();
    GLint u_ViewMatrixLocation = glGetUniformLocation(app.mGraphicsPipelineShaderProgram, "u_ViewMatrix");
    if (u_ViewMatrixLocation >= 0) {
        glUniformMatrix4fv(u_ViewMatrixLocation, 1, false, &viewMatrix[0][0]);
    } else {
        std::println("Could not find u_ViewMatrix");
        exit(EXIT_FAILURE);
    }


    // Projection matrix (in perspective)
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), (float)app.mScreenWidth / (float)app.mScreenHeight,
        0.1f,
        10.0f);

    // Retrieve our location of our Model Matrix
    GLint u_ProjectionLocation = glGetUniformLocation(app.mGraphicsPipelineShaderProgram, "u_Projection");
    if (u_ProjectionLocation >= 0) {
        glUniformMatrix4fv(u_ProjectionLocation, 1, false, &projection[0][0]);
    } else {
        std::println("Could not find u_Projection");
        exit(EXIT_FAILURE);
    }
}

/**
 * Delete a mesh from GPU memory
 */
void MeshDelete(Mesh3D* mesh) {
    // Delete our OpenGL objects
    glDeleteBuffers(1, &mesh->mVertexBufferObject);
    glDeleteVertexArrays(1, &mesh->mVertexBufferObject);
}