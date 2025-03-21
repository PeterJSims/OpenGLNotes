//
// Created by Peter Sims on 3/20/25.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>


class Camera {
public:
    Camera();
    glm::mat4 GetViewMatrix() const;

    void MouseLook(int mouseX, int mouseY);
    void MoveForward(float speed);
    void MoveBackward(float speed);
    void MoveLeft(float speed);
    void MoveRight(float speed);

private:
    glm::vec3 mEye;
    glm::vec3 mViewDirection;
    glm::vec3 mUpVector;

    glm::vec2 mOldMousePosition;
};


#endif //CAMERA_H
