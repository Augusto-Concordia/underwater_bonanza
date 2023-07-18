#include "VisualLine.h"

VisualLine::VisualLine(glm::vec3 _start, glm::vec3 _end, Shader::Descriptor _descriptor) : VisualObject(_start, glm::vec3(0.0f), glm::vec3(1.0f), _descriptor) {
    position = _start;
    end = _end;

    vertices = {
            position.x,
            position.y,
            position.z,
            end.x,
            end.y,
            end.z
    };

    indices = {
            0, 1
    };

    VisualObject::SetupGlBuffersVerticesOnly();
}

void VisualLine::Draw(const glm::mat4& _viewProjection,  const glm::vec3 &_cameraPosition, int _renderMode) {
    glm::mat4 model_matrix = glm::mat4(1.0f);

    DrawFromMatrix(_viewProjection, _cameraPosition, model_matrix, _renderMode);
}

void VisualLine::DrawFromMatrix(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition,
                                const glm::mat4 &_transformMatrix, int _renderMode) {
    //bind the vertex array to draw
    glBindVertexArray(vertex_array_o);

    shader->Use();
    shader->SetModelMatrix(_transformMatrix);
    shader->SetViewProjectionMatrix(_viewProjection);

    shader->SetVec3("u_color", shader_descriptor.color.r, shader_descriptor.color.g, shader_descriptor.color.b);
    shader->SetFloat("u_alpha", shader_descriptor.alpha);

    glLineWidth(shader_descriptor.line_thickness);
    glPointSize(shader_descriptor.point_size);

    //draw vertices according to their indices
    glDrawElements(_renderMode, indices.size(), GL_UNSIGNED_INT, nullptr);
}