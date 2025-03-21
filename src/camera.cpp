//
// Created by Peter Sims on 3/20/25.
//

#include "camera.h"
#include <print>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

Camera::Camera() {
    // Assume the view is placed at the origin
    mEye = glm::vec3(0.0f, 0.0f, 0.0f);
    // Starting view direction for starting at the world
    mViewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
    // Assume a perfect plane
    mUpVector = glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(mEye, mEye + mViewDirection, mUpVector);
}

void Camera::MouseLook(int mouseX, int mouseY) {
    const glm::vec2 currentMousePosition = glm::vec2(mouseX, mouseY);

    static bool firstLook = true;
    if (firstLook) {
        mOldMousePosition = currentMousePosition;
        firstLook = false;
    }


    const glm::vec2 mouseDelta = mOldMousePosition - currentMousePosition;
    mViewDirection = glm::rotate(mViewDirection, glm::radians(mouseDelta.x), mUpVector);

    mOldMousePosition = currentMousePosition;
}


void Camera::MoveForward(const float speed) {
    mEye += (mViewDirection * speed);
}

void Camera::MoveBackward(const float speed) {
    mEye -= (mViewDirection * speed);
}

void Camera::MoveLeft(const float speed) {
    glm::vec3 rightVector = glm::cross(mViewDirection, mUpVector);

    mEye -= rightVector * speed;
}

void Camera::MoveRight(const float speed) {
    glm::vec3 rightVector = glm::cross(mViewDirection, mUpVector);
    mEye += rightVector * speed;
}
