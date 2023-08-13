// For information on how this class (and its parent class) work, see VisualObject.h

#pragma once

#include <memory>
#include <vector>
#include "glm/vec3.hpp"
#include "Components/Shader.h"
#include "VisualObject.h"
#include "assimp/scene.h"

class VisualModel : public VisualObject
{
public:
    VisualModel(const std::string &_path, glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale, Shader::Material  = Shader::Material());

    void Draw(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, float _time = 0.0f, int _renderMode = GL_TRIANGLES, const Shader::Material *_material = nullptr) override;
    void DrawFromMatrix(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, const glm::mat4 &_transformMatrix,  float _time = 0.0f, int _renderMode = GL_TRIANGLES, const Shader::Material *_material = nullptr) override;

    void LoadModel(const std::string &_path);
    void ProcessNode(aiNode *node, const aiScene *scene);
    void ProcessMesh(aiMesh *_mesh, const aiScene *_scene);
};
