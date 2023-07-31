// For information on how this class (and its parent class) work, see VisualObject.h

#pragma once

#include <vector>
#include "glm/vec3.hpp"
#include "glad/glad.h"
#include "Components/Shader.h"
#include "Components/Camera.h"
#include "VisualObject.h"

class VisualGrid : public VisualObject
{
public:
    // Size of each cell
    float cell_size;
    int width, height;

public:
    VisualGrid(int _width, int _height, float _cellSize = 1.0f, glm::vec3 _position = glm::vec3(0.0f), glm::vec3 _rotation = glm::vec3(0.0f), Shader::Material _material = Shader::Material());

    void Draw(const glm::mat4 &viewProjection, const glm::vec3 &_cameraPosition, int _renderMode = GL_LINES, const Shader::Material *_material = nullptr) override;
    void DrawFromMatrix(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, const glm::mat4 &_transformMatrix, int _renderMode = GL_LINES, const Shader::Material *_material = nullptr) override;
};