#include "VisualCube.h"


#include <utility>
#include "Utility/Transform.hpp"

VisualCube::VisualCube(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale, glm::vec3 _transformOffset, Shader::Material _material) : VisualObject(_position, _rotation, _scale, std::move(_material))
{
    // vertices with their normals
    vertices = {
            // top face, top triangle
            -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
            0.5f, -0.5f, 0.5f,      0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
            0.5f, 0.5f, 0.5f,       0.0f, 0.0f, 1.0f,   1.0f, 1.0f,

            // top face, bottom triangle
            -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
            0.5f, 0.5f, 0.5f,       0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
            -0.5f, 0.5f, 0.5f,      0.0f, 0.0f, 1.0f,   0.0f, 1.0f,

            // right side, top triangle
            0.5f, 0.5f, 0.5f,       1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
            0.5f, -0.5f, 0.5f,      1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
            0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 0.0f,   1.0f, 1.0f,

            // right side, bottom triangle
            0.5f, 0.5f, 0.5f,       1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
            0.5f, 0.5f, -0.5f,      1.0f, 0.0f, 0.0f,   0.0f, 1.0f,

            // front side, top triangle
            -0.5f, 0.5f, 0.5f,      0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
            0.5f, 0.5f, 0.5f,       0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
            0.5f, 0.5f, -0.5f,      0.0f, 1.0f, 0.0f,   1.0f, 1.0f,

            // front side, bottom triangle
            -0.5f, 0.5f, 0.5f,      0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
            0.5f, 0.5f, -0.5f,      0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f,     0.0f, 1.0f, 0.0f,   0.0f, 1.0f,

            // left side, top triangle
            -0.5f, 0.5f, 0.5f,      -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
            -0.5f, -0.5f, 0.5f,     -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,    -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,

            // left side, bottom triangle
            -0.5f, 0.5f, 0.5f,      -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,    -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f,     -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,

            // back side, top triangle
            -0.5f, -0.5f, -0.5f,    0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,     0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
            0.5f, -0.5f, 0.5f,      0.0f, -1.0f, 0.0f,  1.0f, 1.0f,

            // back side, bottom triangle
            -0.5f, -0.5f, -0.5f,    0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
            0.5f, -0.5f, 0.5f,      0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
            -0.5f, -0.5f, 0.5f,     0.0f, -1.0f, 0.0f,  0.0f, 1.0f,

            // bottom face, top triangle
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,     0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
            0.5f, 0.5f, -0.5f,      0.0f, 0.0f, -1.0f,  1.0f, 1.0f,

            // bottom face, bottom triangle
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
            0.5f, 0.5f, -0.5f,      0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f,     0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
    };

    for (int i = 0; i < vertices.size(); i += 8)
    {
        vertices[i] += _transformOffset.x;
        vertices[i + 1] += _transformOffset.y;
        vertices[i + 2] += _transformOffset.z;
    }

    VisualObject::SetupGlBuffersVerticesNormalsUvs();
}

void VisualCube::Draw(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition,  float _time, int _renderMode, const Shader::Material *material)
{
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
    model_matrix = Transform::RotateDegrees(model_matrix, rotation);
    model_matrix = glm::scale(model_matrix, scale);

    DrawFromMatrix(_viewProjection, _cameraPosition, model_matrix, _time, _renderMode, material);
}

void VisualCube::DrawFromMatrix(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, const glm::mat4 &_transformMatrix,  float _time, int _renderMode, const Shader::Material *_material)
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

    // object texture mapping & consumption
    current_material->texture->UseSingle(GL_TEXTURE1);
    current_material->shader->SetFloatFast("u_texture_influence", current_material->texture_influence);
    current_material->shader->SetTexture("u_texture", 1);
    current_material->shader->SetVec2("u_texture_tiling", current_material->texture_tiling);

    // caustics texture mapping & consumption
    // texture should be enabled by the screen object at the GL_TEXTURE0 + 18 texture unit
    current_material->shader->SetTexture("u_caustics_texture", 18);

    current_material->shader->SetInt("u_instanced", 0);

    // line & point properties
    glLineWidth(current_material->line_thickness);
    glPointSize(current_material->point_size);

    // draw vertices according to their indices
    glDrawArrays(_renderMode, 0, vertices.size());

    // clear the current texture
    current_material->texture->Clear();
}

void VisualCube::SetupGlBuffersInstanced() {
    // vertices with their normals
    VisualCube::instanced_vertices = {
            // top face, top triangle
            -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
            0.5f, -0.5f, 0.5f,      0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
            0.5f, 0.5f, 0.5f,       0.0f, 0.0f, 1.0f,   1.0f, 1.0f,

            // top face, bottom triangle
            -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
            0.5f, 0.5f, 0.5f,       0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
            -0.5f, 0.5f, 0.5f,      0.0f, 0.0f, 1.0f,   0.0f, 1.0f,

            // right side, top triangle
            0.5f, 0.5f, 0.5f,       1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
            0.5f, -0.5f, 0.5f,      1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
            0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 0.0f,   1.0f, 1.0f,

            // right side, bottom triangle
            0.5f, 0.5f, 0.5f,       1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
            0.5f, 0.5f, -0.5f,      1.0f, 0.0f, 0.0f,   0.0f, 1.0f,

            // front side, top triangle
            -0.5f, 0.5f, 0.5f,      0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
            0.5f, 0.5f, 0.5f,       0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
            0.5f, 0.5f, -0.5f,      0.0f, 1.0f, 0.0f,   1.0f, 1.0f,

            // front side, bottom triangle
            -0.5f, 0.5f, 0.5f,      0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
            0.5f, 0.5f, -0.5f,      0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f,     0.0f, 1.0f, 0.0f,   0.0f, 1.0f,

            // left side, top triangle
            -0.5f, 0.5f, 0.5f,      -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
            -0.5f, -0.5f, 0.5f,     -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,    -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,

            // left side, bottom triangle
            -0.5f, 0.5f, 0.5f,      -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,    -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f,     -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,

            // back side, top triangle
            -0.5f, -0.5f, -0.5f,    0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,     0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
            0.5f, -0.5f, 0.5f,      0.0f, -1.0f, 0.0f,  1.0f, 1.0f,

            // back side, bottom triangle
            -0.5f, -0.5f, -0.5f,    0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
            0.5f, -0.5f, 0.5f,      0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
            -0.5f, -0.5f, 0.5f,     0.0f, -1.0f, 0.0f,  0.0f, 1.0f,

            // bottom face, top triangle
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,     0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
            0.5f, 0.5f, -0.5f,      0.0f, 0.0f, -1.0f,  1.0f, 1.0f,

            // bottom face, bottom triangle
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
            0.5f, 0.5f, -0.5f,      0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f,     0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
    };

    //generate and bind the circles' transform buffer
    glGenBuffers(1, &VisualCube::cube_transforms_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, VisualCube::cube_transforms_buffer);
    //we'll give its transforms later
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //generate and bind the circles' vertex array (VAO)
    glGenVertexArrays(1, &VisualCube::instanced_vertex_array_o);
    glBindVertexArray(VisualCube::instanced_vertex_array_o);

    //generate and bind the circles' VBO
    glGenBuffers(1, &VisualCube::instanced_vertex_buffer_o);
    glBindBuffer(GL_ARRAY_BUFFER, VisualCube::instanced_vertex_buffer_o);
    glBufferData(GL_ARRAY_BUFFER, VisualCube::instanced_vertices.size() * sizeof(float_t),
                 &VisualCube::instanced_vertices.front(), GL_STATIC_DRAW);

    //set vertex attributes pointers (vertex, color, tex)
    //strides are 8 * float_t-size long, because we are including vertex + color + tex coord data in the VAO (Vertex Attribute Object)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float_t), (GLvoid *) nullptr);
    glEnableVertexAttribArray(0);

    //color data has id 1 and starts at a pointer 3 * float_t-size long
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float_t), (GLvoid *) (3 * sizeof(float_t)));
    glEnableVertexAttribArray(1);

    //tex coord data has id 2 and starts at a pointer 6 * float_t-size long
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float_t), (GLvoid *) (6 * sizeof(float_t)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, VisualCube::cube_transforms_buffer);
    //strides are glm::mat4-size long, because we are only including the transform matrix in this buffer
    //however, OpenGL doesn't understand 4x4 matrices, so we need to create 4 vector4
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid *) nullptr);
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid *) (4 * sizeof(float_t)));
    glEnableVertexAttribArray(4);

    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid *) (8 * sizeof(float_t)));
    glEnableVertexAttribArray(5);

    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid *) (12 * sizeof(float_t)));
    glEnableVertexAttribArray(6);

    //manually set each divisor, just to make it clear what is happening (vs. what is default)
    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 0);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
}

void VisualCube::CalculateInstancedFromMatrix(const glm::mat4 &_transformMatrix, const glm::vec3& _color) {

    VisualCube::cube_transforms.push_back(_transformMatrix);
    //VisualCube::cube_colors.push_back(_color);
}

void VisualCube::DrawInstanced(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, float _time, int _renderMode, const Shader::Material *_material) {
    // bind the vertex array to draw
    glBindVertexArray(VisualCube::instanced_vertex_array_o);

    const Shader::Material *current_material = &VisualCube::instanced_material;

    // set the material to use on this frame
    if (_material != nullptr)
        current_material = _material;

    current_material->shader->Use();
    current_material->shader->SetViewProjectionMatrix(_viewProjection);

    // camera properties
    current_material->shader->SetVec3("u_cam_pos", _cameraPosition);

    // lights
    current_material->shader->ApplyLightsToShader(current_material->lights, _time);

    // material properties
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

    current_material->shader->SetInt("u_instanced", 1);

    // line & point properties
    glLineWidth(current_material->line_thickness);
    glPointSize(current_material->point_size);

    //sends object transformation matrices
    glBindBuffer(GL_ARRAY_BUFFER, VisualCube::cube_transforms_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * VisualCube::cube_transforms.size(), VisualCube::cube_transforms.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //draw vertices according to their indices
    glDrawArraysInstanced(GL_TRIANGLES, 0, VisualCube::instanced_vertices.size(), VisualCube::cube_transforms.size());

    // clear the current texture
    current_material->texture->Clear();

    VisualCube::cube_transforms.clear();
    VisualCube::cube_colors.clear();
}
