//
// Created by Peter Sims on 3/21/25.
//

#ifndef MESH_H
#define MESH_H

#include "mesh3d.h"
#include "app.h"

void MeshSetPipeline(Mesh3D* mesh, GLuint pipeline);
void MeshCreate(Mesh3D* mesh);
void MeshDraw(const Mesh3D* mesh);
void MeshUpdate(const Mesh3D* mesh, const App& app);
void MeshDelete(Mesh3D* mesh);


#endif //MESH_H
