#include "VisualCube.h"
#include "Utility/Transform.hpp"

VisualCube::VisualCube(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale, glm::vec3 _transformOffset, Shader::Descriptor _descriptor) : VisualObject( _position, _rotation, _scale, _descriptor) {
    //vertices with their normals
    vertices = {
            //top face, top triangle
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

            //top face, bottom triangle
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

            //right side, top triangle
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,

            //right side, bottom triangle
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,

            //front side, top triangle
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,

            //front side, bottom triangle
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,

            //left side, top triangle
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,

            //left side, bottom triangle
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,

            //back side, top triangle
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

            //back side, bottom triangle
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

            //bottom face, top triangle
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

            //bottom face, bottom triangle
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
    };

    for (int i = 0; i < vertices.size(); i += 6) {
        vertices[i] += _transformOffset.x;
        vertices[i + 1] += _transformOffset.y;
        vertices[i + 2] += _transformOffset.z;
    }

    VisualObject::SetupGlBuffersVerticesAndNormalsOnlyNoIndices();
}

void VisualCube::Draw(const glm::mat4& _viewProjection, const glm::vec3 &_cameraPosition, int _renderMode) {
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
    model_matrix = Transforms::RotateDegrees(model_matrix, rotation);
    model_matrix = glm::scale(model_matrix, scale);

    DrawFromMatrix(_viewProjection, _cameraPosition, model_matrix, _renderMode);
}

void VisualCube::DrawFromMatrix(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, const glm::mat4 &_transformMatrix, int _renderMode) {
    //bind the vertex array to draw
    glBindVertexArray(vertex_array_o);

    shader->Use();
    shader->SetModelMatrix(_transformMatrix);
    shader->SetViewProjectionMatrix(_viewProjection);

    shader->SetVec3("u_color", shader_descriptor.color);
    shader->SetFloat("u_alpha", shader_descriptor.alpha);

    shader->SetVec3("u_cam_pos", _cameraPosition);

    shader->SetVec3("u_light_pos", shader_descriptor.light_position);
    shader->SetVec3("u_light_color", shader_descriptor.light_color);

    shader->SetFloat("u_ambient_strength", shader_descriptor.ambient_strength);
    shader->SetFloat("u_specular_strength", shader_descriptor.specular_strength);
    shader->SetInt("u_shininess", shader_descriptor.shininess);

    glLineWidth(shader_descriptor.line_thickness);
    glPointSize(shader_descriptor.point_size);

    //draw vertices according to their indices
    glDrawArrays(_renderMode, 0, vertices.size());
}
