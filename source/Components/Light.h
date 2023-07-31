#pragma once

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

class Light {
public:
    float ambient_strength = 0.1f;
    float specular_strength = 0.5f;

    inline static int LIGHTMAP_SIZE = 2048;
    inline constexpr static float FOV = 125.0f;
    inline constexpr static float NEAR_PLANE = 0.1f;
    inline constexpr static float FAR_PLANE = 400.0f;

private:
    glm::mat4 view_matrix = glm::mat4(1.0f);
    glm::mat4 projection_matrix = glm::mat4(1.0f);

    glm::vec3 target = glm::vec3(0.0f);
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 color = glm::vec3(1.0f);

    glm::vec3 light_up;
    glm::vec3 light_right;
    glm::vec3 light_forward;

public:
    Light() = default;
    Light(glm::vec3 _position, glm::vec3 _color, float _ambientStrength, float _specularStrength);

    void SetPosition(const glm::vec3& _position);
    void SetTarget(const glm::vec3& _target);

    [[nodiscard]] glm::vec3 GetPosition() const;
    [[nodiscard]] glm::vec3 GetColor() const;
    [[nodiscard]] glm::mat4 GetViewProjection() const;

private:
    void UpdateView(); //for when the camera's rotation changes
    void UpdateProjection(); //for when the camera's viewport changes (mainly)
};
