#include "Camera.h"

Camera::Camera(int shaderProgram, int width, int height, glm::vec3 cameraPos, float fov, float near_plane, float far_plane)
    : shaderProgram(shaderProgram), width(width), height(height), cameraPos(cameraPos),
      fov(fov), near_plane(near_plane), far_plane(far_plane), distance_from(glm::length(cameraPos)) {
}

void Camera::updateCameraMatrix() {
    // initializes matrices
    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, upVector);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)width/ (float)height, near_plane, far_plane);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void Camera::processCameraInputs(GLFWwindow *window) {
    const float sensitivity = 0.5f;
    double xCursorPos, yCursorPos;
    glfwGetCursorPos(window, &xCursorPos, &yCursorPos);

    float deltaX = static_cast<float>(xCursorPos - lastXPos);
    float deltaY = static_cast<float>(yCursorPos - lastYPos);

    lastXPos = xCursorPos;
    lastYPos = yCursorPos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        panCamera(deltaX * sensitivity);
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        tiltCamera(deltaY * sensitivity);
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        cameraPos.x += 0.5f;
        cameraTarget.x += 0.5f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        cameraPos.x -= 0.5f;
        cameraTarget.x -= 0.5f;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        cameraPos.y += 0.5f;
        cameraTarget.y += 0.5f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        cameraPos.y -= 0.5f;
        cameraTarget.y -= 0.5f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            cameraPos.z -= 0.5f;
            cameraTarget.z -= 0.5f;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            cameraPos.z += 0.5f;
            cameraTarget.z += 0.5f;
        }
    }

    updateCameraMatrix();
}

void Camera::panCamera(float deltaX) {
    float angle = deltaX;
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), upVector);
    cameraTarget = glm::vec3(rotationMatrix * glm::vec4(cameraTarget, 1.0f));
}

void Camera::tiltCamera(float deltaY) {
    float angle = deltaY;
    glm::vec3 rightVector = glm::cross(cameraTarget, upVector);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), rightVector);
    cameraTarget = glm::vec3(rotationMatrix * glm::vec4(cameraTarget, 1.0f));
}