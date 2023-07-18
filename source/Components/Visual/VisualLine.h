#pragma once

#include <memory>
#include <vector>
#include "glm/vec3.hpp"
#include "Components/Shader.h"
#include "VisualObject.h"

class VisualLine : public VisualObject {
public:
    glm::vec3 end;

public:
    explicit VisualLine(glm::vec3 _start = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 _end = glm::vec3(1.0f, 1.0f, 1.0f), Shader::Descriptor _descriptor = Shader::Descriptor());

    void Draw(const glm::mat4& _viewProjection,  const glm::vec3 &_cameraPosition, int _renderMode = GL_LINES) override;
    void DrawFromMatrix(const glm::mat4& _viewProjection, const glm::vec3 &_cameraPosition, const glm::mat4& _transformMatrix, int _renderMode = GL_LINES) override;
};
