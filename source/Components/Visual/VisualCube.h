// For information on how this class (and its parent class) work, see VisualObject.h

#pragma once

#include <memory>
#include <vector>
#include "glm/vec3.hpp"
#include "Components/Shader.h"
#include "VisualObject.h"

class VisualCube : public VisualObject
{
private:
    inline static Shader::Material instanced_material;

    inline static GLuint cube_transforms_buffer = 0;
    inline static GLuint cube_colors_buffer = 0;
    inline static GLuint instanced_vertex_buffer_o = 0;
    inline static GLuint instanced_vertex_array_o = 0;

    inline static std::vector<float> instanced_vertices = std::vector<float>();
    inline static std::vector<int> instanced_indices = std::vector<int>();

    inline static std::vector<glm::mat4> cube_transforms = std::vector<glm::mat4>();
    inline static std::vector<glm::vec3> cube_colors = std::vector<glm::vec3>();

public:
    explicit VisualCube(glm::vec3 _position = glm::vec3(0.0f), glm::vec3 _rotation = glm::vec3(0.0f), glm::vec3 _scale = glm::vec3(1.0f), glm::vec3 _transformOffset = glm::vec3(0.0f), Shader::Material _material = Shader::Material());

    void Draw(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition,  float _time = 0.0f, int _renderMode = GL_TRIANGLES, const Shader::Material *_material = nullptr) override;
    void DrawFromMatrix(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, const glm::mat4 &_transformMatrix,  float _time = 0.0f, int _renderMode = GL_TRIANGLES, const Shader::Material *_material = nullptr) override;

    static void CalculateInstancedFromMatrix(const glm::mat4 &_transformMatrix, const glm::vec3& _color);
    static void DrawInstanced(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, float _time = 0.0f, int _renderMode = GL_TRIANGLES, const Shader::Material *_material = nullptr);

    static void SetupGlBuffersInstanced();
};
