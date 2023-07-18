#pragma once

#include <iostream>
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

class Camera {
public:
    enum Translation {
        CAMERA_FORWARD,
        CAMERA_BACKWARD,
        FORWARD,
        BACKWARD,
        CAMERA_UP,
        CAMERA_DOWN,
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

    enum Rotation {
        POSITIVE_PITCH,
        NEGATIVE_PITCH,
        POSITIVE_ROLL,
        NEGATIVE_ROLL,
        POSITIVE_YAW,
        NEGATIVE_YAW
    };

    enum Orbitation {
        ORBIT_UP,
        ORBIT_DOWN,
        ORBIT_LEFT,
        ORBIT_RIGHT
    };

    float translation_speed = 4.5f;
    float rotation_speed = 5.0f;
    float orbit_speed = 24.0f;

    float distance_to_target = 0.0f; //up-to-date distance to the target (for quicker camera navigation calculations)

    const float FOV = 75.0f;
    const float NEAR_PLANE = 0.1f;
    const float FAR_PLANE = 100.0f;

public:
    Camera();

    Camera(glm::vec3 _position, glm::vec3 _target, float _viewportWidth = 1280, float _viewportHeight = 720);

    void OneAxisMove(Translation _translation, float _delta);
    void OneAxisRotate(Rotation _rotation, float _delta);
    void OneAxisOrbit(Orbitation _orbitation, float _delta);

    void SetViewportSize(float _width, float _height);
    void SetDefaultPositionAndTarget();

    [[nodiscard]] glm::vec3 GetPosition() const;
    [[nodiscard]] glm::mat4 GetViewProjection() const;

private:
    void UpdateProjection(); //for when the camera's viewport changes (mainly)
    void UpdateView(); //for when the camera's rotation changes

private:
    glm::vec3 cam_position, default_cam_position;
    glm::vec3 cam_target, default_cam_target;

    glm::vec3 cam_up;
    glm::vec3 cam_right;
    glm::vec3 cam_forward;

    float viewport_height;
    float viewport_width;

    glm::mat4 view_matrix = glm::mat4(1.0f);
    glm::mat4 projection_matrix = glm::mat4(1.0f);
};
