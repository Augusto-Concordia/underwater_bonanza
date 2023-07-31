// For information on how this class (and its parent class) work, see VisualObject.h

#pragma once

#include <memory>
#include <vector>
#include "glm/vec3.hpp"
#include "Components/Shader.h"
#include "Visual/VisualObject.h"

class Screen : public VisualObject
{
public:
    explicit Screen(Shader::Material _material = Shader::Material());

    void Draw(const glm::mat4 &_viewProjection = glm::mat4(1.0f), const glm::vec3 &_cameraPosition = glm::vec3(0.0f), int _renderMode = GL_TRIANGLES, const Shader::Material *_material = nullptr) override;
    void DrawFromMatrix(const glm::mat4 &_viewProjection = glm::mat4(1.0f), const glm::vec3 &_cameraPosition = glm::vec3(0.0f), const glm::mat4 &_transformMatrix = glm::mat4(1.0f), int _renderMode = GL_TRIANGLES, const Shader::Material *_material = nullptr) override;
};
