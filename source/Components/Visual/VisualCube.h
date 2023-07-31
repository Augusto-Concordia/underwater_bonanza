// For information on how this class (and its parent class) work, see VisualObject.h

#pragma once

#include <memory>
#include <vector>
#include "glm/vec3.hpp"
#include "Components/Shader.h"
#include "VisualObject.h"

class VisualCube : public VisualObject
{
public:
    explicit VisualCube(glm::vec3 _position = glm::vec3(0.0f), glm::vec3 _rotation = glm::vec3(0.0f), glm::vec3 _scale = glm::vec3(1.0f), glm::vec3 _transformOffset = glm::vec3(0.0f), Shader::Material _material = Shader::Material());

    void Draw(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, int _renderMode = GL_TRIANGLES, const Shader::Material *_material = nullptr) override;
    void DrawFromMatrix(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, const glm::mat4 &_transformMatrix, int _renderMode = GL_TRIANGLES, const Shader::Material *_material = nullptr) override;
};
