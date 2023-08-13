#include "VisualModel.h"

#include <utility>
#include "Utility/Transform.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

VisualModel::VisualModel(const std::string& _path, glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale, Shader::Material _material) : VisualObject(_position, _rotation, _scale, std::move(_material))
{
    LoadModel(_path);

    VisualObject::SetupGlBuffersVerticesNormalsUvs();
}

void VisualModel::LoadModel(const std::string& _path)
{
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }

    ProcessNode(scene->mRootNode, scene);
}

void VisualModel::ProcessNode(aiNode *node, const aiScene *scene)
{
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene);
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

void VisualModel::ProcessMesh(aiMesh *_mesh, const aiScene *_scene)
{
    for(unsigned int i = 0; i < _mesh->mNumVertices; i++)
    {
        vertices.push_back(_mesh->mVertices[i].x);
        vertices.push_back(_mesh->mVertices[i].y);
        vertices.push_back(_mesh->mVertices[i].z);

        vertices.push_back(_mesh->mNormals[i].x);
        vertices.push_back(_mesh->mNormals[i].y);
        vertices.push_back(_mesh->mNormals[i].z);

        if(_mesh->mTextureCoords[0])
        {
            vertices.push_back(_mesh->mTextureCoords[0][i].x);
            vertices.push_back(_mesh->mTextureCoords[0][i].y);
        }
        else
        {
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }
    }
}

void VisualModel::Draw(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition,  float _time, int _renderMode, const Shader::Material *material)
{
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
    model_matrix = Transform::RotateDegrees(model_matrix, rotation);
    model_matrix = glm::scale(model_matrix, scale);

    DrawFromMatrix(_viewProjection, _cameraPosition, model_matrix, _time, _renderMode, material);
}

void VisualModel::DrawFromMatrix(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, const glm::mat4 &_transformMatrix,  float _time, int _renderMode, const Shader::Material *_material)
{
    // bind the vertex array to draw
    glBindVertexArray(vertex_array_o);

    const Shader::Material *current_material = &material;

    // set the material to use on this frame
    if (_material != nullptr)
        current_material = _material;

    current_material->shader->Use();
    current_material->shader->SetModelMatrix(_transformMatrix);
    current_material->shader->SetViewProjectionMatrix(_viewProjection);

    // camera properties
    current_material->shader->SetVec3("u_cam_pos", _cameraPosition);

    // lights
    current_material->shader->ApplyLightsToShader(current_material->lights, _time);

    // material properties
    current_material->shader->SetVec3("u_color", current_material->color);
    current_material->shader->SetFloat("u_alpha", current_material->alpha);
    current_material->shader->SetInt("u_shininess", current_material->shininess);

    // caustics texture mapping & consumption
    // texture should be enabled by the screen object at the GL_TEXTURE0 + 18 texture unit
    current_material->shader->SetTexture("u_caustics_texture", 18);

    // line & point properties
    glLineWidth(current_material->line_thickness);
    glPointSize(current_material->point_size);

    // draw vertices according to their indices
    glDrawArrays(_renderMode, 0, vertices.size());

    // clear the current texture
    current_material->texture->Clear();
}
