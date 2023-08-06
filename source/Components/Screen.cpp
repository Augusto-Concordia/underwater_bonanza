#include "Screen.h"

#include <utility>
#include "Utility/Transform.hpp"
#include "Utility/Graphics.hpp"

Screen::Screen(GLsizei _width, GLsizei _height, GLuint _textureUnit, Shader::Material _material) : VisualObject(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), std::move(_material))
{
    original_screen_width = screen_width = _width;
    original_screen_height = screen_height = _height;
    screen_base_texture_unit = _textureUnit;

    // quad vertices with their uvs
    vertices = {
        -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,
        1.0f, -1.0f, 0.0f,      1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,      1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f,     0.0f, 1.0f,
    };

    indices = {
        0, 1, 2,
        0, 2, 3
    };

    VisualObject::SetupGlBuffersVerticesUvsWithIndices();
}

void Screen::Init() {
    // initializes the screen framebuffer
    glGenFramebuffers(1, &screen_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, screen_framebuffer);

    // COLOR

    // initializes the screen color texture
    glGenTextures(1, &screen_color_texture);
    glBindTexture(GL_TEXTURE_2D, screen_color_texture);

    // sets the texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // sets the texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // attaches the render color texture to the framebuffer
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, screen_color_texture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // DEPTH

    // initializes the screen depth texture
    glGenTextures(1, &screen_depth_texture);
    glBindTexture(GL_TEXTURE_2D, screen_depth_texture);

    // sets the texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // sets the render buffer data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, screen_width, screen_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // attaches the render color texture to the framebuffer
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, screen_depth_texture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // disable color draw & read buffer for this framebuffer
    GLenum draw_buffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, draw_buffers);

    Graphics::ValidateFramebufferStatus();
}

void Screen::Bind() const {
    // binds the screen framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, screen_framebuffer);

    // sets the viewport to the desired screen size
    glViewport(0, 0, screen_width, screen_height);

    // clears the color & depth canvas to black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Screen::Unbind() const {
    // unbinds the screen framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Screen::ResizeCallback(GLsizei _width, GLsizei _height) {
    screen_width = _width;
    screen_height = _height;

    // resize the screen framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, screen_framebuffer);

    // resize the screen color texture
    glBindTexture(GL_TEXTURE_2D, screen_color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // resize the screen depth texture
    glBindTexture(GL_TEXTURE_2D, screen_depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, screen_width, screen_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
}

void Screen::Draw(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, int _renderMode, const Shader::Material *material)
{
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
    model_matrix = Transform::RotateDegrees(model_matrix, rotation);
    model_matrix = glm::scale(model_matrix, scale);

    DrawFromMatrix(_viewProjection, _cameraPosition, model_matrix, _renderMode, material);
}

void Screen::DrawFromMatrix(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, const glm::mat4 &_transformMatrix, int _renderMode, const Shader::Material *_material)
{
    // bind the vertex array to draw
    glBindVertexArray(vertex_array_o);

    const Shader::Material *current_material = &material;

    // set the material to use on this frame
    if (_material != nullptr)
        current_material = _material;

    current_material->shader->Use();

    //current_material->shader->SetMat4("u_screen", glm::scale(glm::mat4(1.0f), 1.0f / glm::vec3(original_screen_width / (float)screen_width, original_screen_height / (float)screen_height, 1.0f)));

    //current_material->shader->SetVec2("u_uv_scalar", (float)screen_width, (float)screen_height);

    glActiveTexture(GL_TEXTURE0 + screen_base_texture_unit);
    glBindTexture(GL_TEXTURE_2D, screen_color_texture);
    current_material->shader->SetTexture("u_scene_color_texture", (GLint)screen_base_texture_unit);

    glActiveTexture(GL_TEXTURE0 + screen_base_texture_unit + 1);
    glBindTexture(GL_TEXTURE_2D, screen_depth_texture);
    current_material->shader->SetTexture("u_scene_depth_texture", (GLint)screen_base_texture_unit + 1);

    // draw vertices according to their indices
    glDrawElements(_renderMode, indices.size(), GL_UNSIGNED_INT, nullptr);
}
