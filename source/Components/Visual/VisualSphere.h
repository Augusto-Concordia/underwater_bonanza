// For information on how this class (and its parent class) work, see VisualObject.h

#pragma once

#include <memory>
#include <vector>
#include "glm/vec3.hpp"
#include "Components/Shader.h"
#include "VisualObject.h"

class VisualSphere : public VisualObject
{
public:
    float radius;
    int subdivisions;

    explicit VisualSphere(float radius = 1.0f, int subdivisions = 1, glm::vec3 _position = glm::vec3(0.0f), glm::vec3 _rotation = glm::vec3(0.0f), glm::vec3 _scale = glm::vec3(1.0f), Shader::Material _material = Shader::Material());

    void subdivideTriangles();

    glm::vec3 normalizeVertice(float vx, float vy, float vz);
    glm::vec3 computeFaceNormals(glm::vec3 v);
    glm::vec2 computeVertexTexture(glm::vec3 v);

    void Draw(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, int _renderMode = GL_TRIANGLES, const Shader::Material *_material = nullptr) override;
    void DrawFromMatrix(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, const glm::mat4 &_transformMatrix, int _renderMode = GL_TRIANGLES, const Shader::Material *_material = nullptr) override;
};
