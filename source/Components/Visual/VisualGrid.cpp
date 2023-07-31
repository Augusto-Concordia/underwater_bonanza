#include "VisualGrid.h"
#include "Utility/Math.hpp"
#include "Utility/Transform.hpp"

VisualGrid::VisualGrid(int _width, int _height, float _cellSize, glm::vec3 _position, glm::vec3 _rotation, Shader::Material _material) : VisualObject(_position, _rotation, glm::vec3(0.0f), _material)
{
    cell_size = _cellSize;
    width = _width;
    height = _height;

    vertices = std::vector<float>();
    indices = std::vector<int>();

    // generates vertices for the far side of the grid
    for (int i = 0; i <= this->width; ++i)
    {
        vertices.push_back(Math::Map((float)i, 0, (float)this->width, -1.0f, 1.0f));
        vertices.push_back(-1.0f);
        vertices.push_back(0.0f);
    }

    // generates vertices for the right side of the grid
    for (int i = 1; i <= this->height; ++i)
    {
        vertices.push_back(1.0f);
        vertices.push_back(Math::Map((float)i, 0, (float)this->height, -1.0f, 1.0f));
        vertices.push_back(0.0f);
    }

    // generates vertices for the near side of the grid
    for (int i = this->width - 1; i >= 0; --i)
    {
        vertices.push_back(Math::Map((float)i, 0, (float)this->width, -1.0f, 1.0f));
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
    }

    // generates vertices for the left side of the grid
    for (int i = this->height - 1; i > 0; --i)
    {
        vertices.push_back(-1.0f);
        vertices.push_back(Math::Map((float)i, 0, (float)this->height, -1.0f, 1.0f));
        vertices.push_back(0.0f);
    }

    const int total_vertices = 2 * this->width + 2 * this->height; // number of vertices on the perimeter of the grid
    const int three_quarter_loop = total_vertices - this->height;  // number of vertices to do a quarter turn of the perimeter

    // generates indices for the vertical lines of the grid
    for (int i = 0; i <= this->width; ++i)
    {
        const int top_side_index = i;
        const int bottom_side_index = three_quarter_loop - i;

        indices.push_back(top_side_index);
        indices.push_back(bottom_side_index);
    }

    // generates indices for the horizontal lines of the grid
    for (int i = this->height; i >= 0; --i)
    {
        const int left_side_index = (total_vertices - i) % (total_vertices);
        const int right_side_index = this->width + i;

        indices.push_back(left_side_index);
        indices.push_back(right_side_index);
    }

    VisualObject::SetupGlBuffersVerticesOnly();
}

void VisualGrid::Draw(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, int _renderMode, const Shader::Material *_material)
{
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::scale(model_matrix, glm::vec3((float)width * cell_size / 2, 0.0f, (float)height * cell_size / 2));
    model_matrix = Transform::RotateDegrees(model_matrix, rotation);
    model_matrix = glm::translate(model_matrix, position);

    DrawFromMatrix(_viewProjection, _cameraPosition, model_matrix, _renderMode, _material);
}

void VisualGrid::DrawFromMatrix(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition,
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

    current_material->shader->SetVec3("u_color", current_material->color.x, current_material->color.y, current_material->color.z);
    current_material->shader->SetFloat("u_alpha", current_material->alpha);

    glLineWidth(current_material->line_thickness);
    glPointSize(current_material->point_size);

    // draw vertices according to their indices
    glDrawElements(_renderMode, indices.size(), GL_UNSIGNED_INT, nullptr);
}
