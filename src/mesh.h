//
// Created by Peter Sims on 3/21/25.
//

#ifndef MESH_H
#define MESH_H

#include "mesh3d.h"
#include "app.h"

void MeshSetPipeline(Mesh3D* mesh, GLuint pipeline);
void MeshCreate(Mesh3D* mesh);
void MeshDraw(App* app, const Mesh3D* mesh);
void MeshDelete(Mesh3D* mesh);
GLint FindUniformLocation(GLuint pipeline, const GLchar* name);


#endif //MESH_H
