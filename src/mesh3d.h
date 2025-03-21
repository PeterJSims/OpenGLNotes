//
// Created by Peter Sims on 3/21/25.
//

#ifndef MESH3D_H
#define MESH3D_H

#include <glad/glad.h>
#include "transform.h"


struct Mesh3D {

    // OpenGL Objects
    // Vertex Array Object (VAO)
    // Vertex array objects encapsulate all the items needed to render an object.
    // For example, we may have multiple vertex buffer objects (VBO) related to rendering
    // one object. The VAO allows us to set up the OpenGL state to render that object using
    // the correct layout and correct buffers with one call after being setup.
    GLuint mVertexArrayObject{0};

    // Vertex Buffer Object (VBO)
    // Vertex Buffer Objects store information relating to vertices (e.g. positions, normals,
    // textures)
    // VBOs are our mechanism for arranging geometry on the GPU.
    GLuint mVertexBufferObject{0};

    // Index Buffer Object
    // This is used to store the array of indices that we want to draw from
    // when we do indexed drawing.
    GLuint mIndexBufferObject{0};

    // The pipeline used with this mesh

    GLuint mPipeline = 0;

    Transform mTransform;
    // Global offsets for rotations/zoom
    float m_uRotate{0.0f};
    float m_uScale{0.5f};

};


#endif //MESH3D_H
