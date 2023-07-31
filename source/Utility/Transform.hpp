#pragma once

#include "glm/vec3.hpp"

struct Transform {
    constexpr static glm::vec3 FORWARD = glm::vec3 (1.0f, 0.0f, 0.0f);
    constexpr static glm::vec3 RIGHT = glm::vec3 (0.0f, 0.0f, 1.0f);
    constexpr static glm::vec3 UP = glm::vec3 (0.0f, 1.0f, 0.0f);

    static glm::mat4 RotateDegrees(const glm::mat4& _matrix, const glm::vec3& _rotationAngles) {
        //makes it easy to give human-readable angles to each axis of rotation
        glm::mat4 result = glm::rotate(_matrix, glm::radians(_rotationAngles.x), glm::vec3(-1.0f, 0.0f, 0.0f));
        result = glm::rotate(result, glm::radians(_rotationAngles.y), glm::vec3(0.0f, -1.0f, 0.0f));
        result = glm::rotate(result, glm::radians(_rotationAngles.z), glm::vec3(0.0f, 0.0f, -1.0f));

        return result;
    }
};