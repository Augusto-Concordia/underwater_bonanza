#include "Camera.h"
#include "Utility/Transform.hpp"

Camera::Camera() : Camera::Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f)) {}

Camera::Camera(glm::vec3 _position, glm::vec3 _target, float _viewportWidth, float _viewportHeight) {
    cam_position = default_cam_position = _position;
    cam_target = default_cam_target = _target;

    distance_to_target = glm::length(default_cam_position - default_cam_target);

    viewport_width = _viewportWidth;
    viewport_height = _viewportHeight;

    UpdateView();
    UpdateProjection();
}

void Camera::OneAxisMove(Camera::Translation _translation, float _delta) {
    //2 delta variables so that when we are translating into the target (CAMERA_FORWARD & CAMERA_BACKWARD),
    // we don't move the target too
    auto delta_cam = glm::vec3(0), delta_target = glm::vec3(0);

    switch (_translation) {
        case UP:
            delta_target = delta_cam = _delta * translation_speed * Transforms::UP;
            break;
        case DOWN:
            delta_target = delta_cam = -_delta * translation_speed * Transforms::UP;
            break;
        case CAMERA_UP:
            delta_target = delta_cam = -_delta * translation_speed * cam_up;
            break;
        case CAMERA_DOWN:
            delta_target = delta_cam = _delta * translation_speed * cam_up;
            break;
        case LEFT:
            delta_target = delta_cam = _delta * translation_speed * cam_right;
            break;
        case RIGHT:
            delta_target = delta_cam = -_delta * translation_speed * cam_right;
            break;

            //we take the cross because we want to move forward,
            //parallel to the ground (similar to other programs' world space toggle)
        case FORWARD:
            delta_target = delta_cam = _delta * translation_speed * glm::cross(cam_right, Transforms::UP);
            break;
        case BACKWARD:
            delta_target = delta_cam = -_delta * translation_speed * glm::cross(cam_right, Transforms::UP);
            break;

            //camera forward & backward follows the center of the camera's view
        case CAMERA_FORWARD:
            delta_cam = -_delta * translation_speed * cam_forward;
            break;
        case CAMERA_BACKWARD:
            delta_cam = _delta * translation_speed * cam_forward;
            break;
    }

    //if we changed the distance between the camera and the target, recalculate it and apply camera constraints
    if (delta_target != delta_cam)
    {
        distance_to_target = glm::clamp(glm::length(cam_position + delta_cam - cam_target), 0.5f, std::numeric_limits<float>::infinity());
        cam_position = cam_target + cam_forward * distance_to_target;
    }
    //otherwise, move the target and camera accordingly
    else
    {
        cam_target += delta_target;
        cam_position += delta_cam;
    }

    UpdateView();
}

void Camera::OneAxisRotate(Camera::Rotation _rotation, float _delta) {
    glm::vec3 rotation_axis;

    //to easily rotate the camera itself, we create a rotation matrix that creates a rotated vector, and we subtract it from the camera's position
    switch (_rotation) {
        case POSITIVE_PITCH:
            rotation_axis = -cam_right;
            break;
        case NEGATIVE_PITCH:
            rotation_axis = cam_right;
            break;
        case POSITIVE_ROLL:
            rotation_axis = -cam_forward;
            break;
        case NEGATIVE_ROLL:
            rotation_axis = cam_forward;
            break;
        case POSITIVE_YAW:
            rotation_axis = cam_up;
            break;
        case NEGATIVE_YAW:
            rotation_axis = -cam_up;
            break;
    }

    glm::mat4 rotation_matrix = glm::mat4(1);
    rotation_matrix = glm::rotate(rotation_matrix, glm::radians(_delta * rotation_speed), rotation_axis);

    cam_forward = glm::vec3(glm::vec4(cam_forward, 1.0) * rotation_matrix);
    cam_target = cam_position - cam_forward * distance_to_target;

    UpdateView();
}

void Camera::OneAxisOrbit(Camera::Orbitation _orbitation, float _delta) {
    glm::vec3 rotation_axis;

    //to easily orbit the camera, we create a rotation matrix that creates a rotated vector, and we add it to the target's position
    switch (_orbitation) {
        case ORBIT_UP:
            rotation_axis = cam_right;
            break;
        case ORBIT_DOWN:
            rotation_axis = -cam_right;
            break;
        case ORBIT_RIGHT:
            rotation_axis = Transforms::UP;
            break;
        case ORBIT_LEFT:
            rotation_axis = -Transforms::UP;
            break;
    }

    glm::mat4 rotation_matrix = glm::mat4(1);
    rotation_matrix = glm::rotate(rotation_matrix, glm::radians(_delta * orbit_speed), rotation_axis);

    cam_forward = glm::vec3(rotation_matrix * glm::vec4(cam_forward, 1.0));
    cam_position = cam_target + cam_forward * distance_to_target;

    UpdateView();
}

void Camera::SetViewportSize(float _width, float _height) {
    viewport_width = _width;
    viewport_height = _height;

    UpdateProjection();
}

void Camera::SetDefaultPositionAndTarget() {
    cam_position = default_cam_position;
    cam_target = default_cam_target;

    distance_to_target = glm::length(cam_position - cam_target);

    UpdateView();
}

glm::vec3 Camera::GetPosition() const {
    return cam_position;
}

glm::mat4 Camera::GetViewProjection() const {
    return projection_matrix * view_matrix;
}

void Camera::UpdateView() {
    float infinity = std::numeric_limits<float>::infinity();

    //cheap, but effective way of constraining the camera to avoid looking straight up or down
    cam_forward = glm::clamp(glm::normalize(cam_position - cam_target), -glm::vec3(infinity, infinity, 0.94f), glm::vec3(infinity, infinity, 0.94f));
    cam_right = glm::normalize(glm::cross(cam_forward, Transforms::UP));
    cam_up = glm::normalize(glm::cross(cam_right, cam_forward));

    view_matrix = glm::lookAt(cam_position, cam_target, cam_up);
}

void Camera::UpdateProjection() {
    projection_matrix = glm::perspective(glm::radians(Camera::FOV), viewport_width / viewport_height, NEAR_PLANE, FAR_PLANE);
}