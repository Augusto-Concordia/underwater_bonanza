#include "VisualLine.h"

VisualLine::VisualLine(glm::vec3 _start, glm::vec3 _end, Shader::Material _material) : VisualObject(_start, glm::vec3(0.0f), glm::vec3(1.0f), _material)
{
    position = _start;
    end = _end;

    vertices = {
            position.x,
            position.y,
            position.z,
            end.x,
            end.y,
            end.z};

    indices = {
            0, 1};

    VisualObject::SetupGlBuffersVerticesOnly();
}

void VisualLine::Draw(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, int _renderMode, const Shader::Material *_material)
{
    glm::mat4 model_matrix = glm::mat4(1.0f);

    DrawFromMatrix(_viewProjection, _cameraPosition, model_matrix, _renderMode, _material);
}

void VisualLine::DrawFromMatrix(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition,
                                const glm::mat4 &_transformMatrix, int _renderMode, const Shader::Material *_material)
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

    current_material->shader->SetVec3("u_color", current_material->color.r, current_material->color.g, current_material->color.b);
    current_material->shader->SetFloat("u_alpha", current_material->alpha);

    glLineWidth(current_material->line_thickness);
    glPointSize(current_material->point_size);

    // draw vertices according to their indices
    glDrawElements(_renderMode, indices.size(), GL_UNSIGNED_INT, nullptr);
}