#include "VisualTrapezoidal.h"

#include <utility>
#include "Utility/Transform.hpp"

VisualTrapezoidal::VisualTrapezoidal(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale, glm::vec3 _transformOffset, Shader::Material _material) : VisualObject(_position, _rotation, _scale, std::move(_material))
{
    float sizeX = _scale.x;
    float sizeZ = _scale.y;
    float height = _scale.z;

    // vertices with their normals
    vertices = {
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f,
            sizeX, height, -sizeZ, 0.0f, 0.0f, -1.0f,
            1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f,

            sizeX, height, -sizeZ, 0.0f, 0.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,0.0f, 0.0f, -1.0f,
            -sizeX, height, -sizeZ, 0.0f, 0.0f, -1.0f,
            //frontface
            -1.0f, -1.0f, 1.0f,0.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f,0.0f, 0.0f, 1.0f,
            sizeX, height, sizeZ,0.0f, 0.0f, 1.0f,

            sizeX, height, sizeZ, 0.0f, 0.0f, 1.0f,
            -sizeX, height, sizeZ,0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 1.0,0.0f, 0.0f, 1.0f,

            //leftface
            -sizeX, height, sizeZ,  -1.0f, 0.0f, 0.0f,
            -sizeX, height, -sizeZ, -1.0f, 0.0f, 0.0f,
            -1.0f, -1.0f, -1.0f,-1.0f, 0.0f, 0.0f,

            -1.0f, -1.0f, -1.0f,-1.0f, 0.0f, 0.0f,
            -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
            -sizeX, height, sizeZ,  -1.0f, 0.0f, 0.0f,
            //right face
            sizeX, height, sizeZ,  1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, -1.0f,1.0f, 0.0f, 0.0f,
            sizeX, height, -sizeZ, 1.0f, 0.0f, 0.0f,

            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
            sizeX, height, sizeZ,   1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
            //bottom face
            -1.0f, -1.0f, -1.0f,0.0f, -1.0f, 0.0f,
            1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f,

            1.0f, -1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, -1.0f,0.0f, -1.0f, 0.0f,
            //top face
            -sizeX, height, -sizeZ, 0.0f, 1.0f, 0.0f,
            sizeX, height, sizeZ ,  0.0f, 1.0f, 0.0f,
            sizeX, height, -sizeZ,  0.0f, 1.0f, 0.0f,

            sizeX, height, sizeZ , 0.0f,1.0f,0.0f,
            -sizeX, height, -sizeZ,0.0f,1.0f,0.0f,
            -sizeX, height, sizeZ, 0.0f,1.0f,0.0f,
    };

    VisualObject::SetupGlBuffersVerticesNormals();
}

void VisualTrapezoidal::Draw(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition,  float _time, int _renderMode, const Shader::Material *material)
{
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
    model_matrix = Transform::RotateDegrees(model_matrix, rotation);
    model_matrix = glm::scale(model_matrix, scale);

    DrawFromMatrix(_viewProjection, _cameraPosition, model_matrix, _time, _renderMode, material);
}

void VisualTrapezoidal::DrawFromMatrix(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, const glm::mat4 &_transformMatrix,  float _time, int _renderMode, const Shader::Material *_material)
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

    current_material->shader->SetInt("u_instanced", 0);

    // camera properties
    current_material->shader->SetVec3("u_cam_pos", _cameraPosition);

    // lights
    current_material->shader->ApplyLightsToShader(current_material->lights, _time);

    // material properties
    current_material->shader->SetVec3("u_color", current_material->color);
    current_material->shader->SetFloat("u_alpha", current_material->alpha);
    current_material->shader->SetInt("u_shininess", current_material->shininess);

    // object texture mapping & consumption
    current_material->texture->UseSingle(GL_TEXTURE1);
    current_material->shader->SetFloatFast("u_texture_influence", current_material->texture_influence);
    current_material->shader->SetTexture("u_texture", 1);
    current_material->shader->SetVec2("u_texture_tiling", current_material->texture_tiling);

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
