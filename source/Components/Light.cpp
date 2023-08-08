#include "Light.h"
#include "Utility/Transform.hpp"

Light::Light(glm::vec3 _position, glm::vec3 _color, float _ambientStrength, float _specularStrength, Type _type) {
    position = _position;
    target = glm::vec3(0.0f);
    color = _color;
    ambient_strength = _ambientStrength;
    specular_strength = _specularStrength;
    type = _type;

    UpdateView();
    UpdateProjection();
    UpdateAttenuation();
}

void Light::SetPosition(const glm::vec3 &_position) {
    position = _position;

    UpdateView();
}

void Light::SetTarget(const glm::vec3 &_target) {
    target = _target;

    UpdateView();
}

void Light::SetRange(float _range) {
    range = _range;

    UpdateAttenuation();
}

glm::vec3 Light::GetPosition() const {
    return position;
}

glm::vec3 Light::GetTarget() const {
    return target;
}

glm::vec3 Light::GetColor() const {
    return color;
}

glm::vec3 Light::GetLightDirection() const {
    return light_forward;
}

float Light::GetSpotlightCutoff() const {
    return glm::cos(glm::radians(cutoff));
}

glm::mat4 Light::GetViewProjection() const {
    return projection_matrix * view_matrix;
}

void Light::UpdateView() {
    light_forward = glm::normalize(position - target);
    light_right = glm::normalize(glm::cross(light_forward, Transform::UP));
    light_up = glm::normalize(glm::cross(light_right, light_forward));

    view_matrix = glm::lookAt(position, target, light_up);
}

void Light::UpdateProjection() {
    switch (type) {
        case Light::Type::POINT:
        case Light::Type::SPOT:
            projection_matrix = glm::perspective(glm::radians(Light::FOV), (float)Light::LIGHTMAP_SIZE / (float)Light::LIGHTMAP_SIZE, Light::NEAR_PLANE, Light::FAR_PLANE);
            break;
        case Light::Type::DIRECTIONAL:
            projection_matrix = glm::ortho((float)Light::LIGHTMAP_SIZE, (float)Light::LIGHTMAP_SIZE, (float)Light::LIGHTMAP_SIZE, (float)Light::LIGHTMAP_SIZE, Light::NEAR_PLANE, Light::FAR_PLANE);
    }
}

void Light::UpdateAttenuation() {
    attenuation = glm::vec3(1.0f, 4.5f / range, 75.0f / (range * range)); //values from: https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
}