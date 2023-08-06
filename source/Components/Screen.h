// For information on how this class (and its parent class) work, see VisualObject.h

#pragma once

#include <memory>
#include <vector>
#include "glm/vec3.hpp"
#include "Components/Shader.h"
#include "Visual/VisualObject.h"

class Screen : public VisualObject
{
private:
    GLsizei original_screen_width, screen_width = 0;
    GLsizei original_screen_height, screen_height = 0;

    GLuint screen_framebuffer = 0;
    GLuint screen_base_texture_unit = 0;
    GLuint screen_color_texture = 0;
    GLuint screen_depth_texture = 0;

public:
    explicit Screen(GLsizei _width, GLsizei _height, GLuint _textureUnit = 15, Shader::Material _material = Shader::Material());

    void Init();
    void Bind() const;
    void Unbind() const;

    void ResizeCallback(GLsizei _width, GLsizei _height);

    void Draw(const glm::mat4 &_viewProjection = glm::mat4(1.0f), const glm::vec3 &_cameraPosition = glm::vec3(0.0f), int _renderMode = GL_TRIANGLES, const Shader::Material *_material = nullptr) override;
    void DrawFromMatrix(const glm::mat4 &_viewProjection = glm::mat4(1.0f), const glm::vec3 &_cameraPosition = glm::vec3(0.0f), const glm::mat4 &_transformMatrix = glm::mat4(1.0f), int _renderMode = GL_TRIANGLES, const Shader::Material *_material = nullptr) override;
};