#pragma once

#include <map>
#include "Camera.h"
#include "Shader.h"
#include "Visual/VisualGrid.h"
#include "GLFW/glfw3.h"
#include "Components/Visual/VisualLine.h"
#include "Components/Visual/VisualCube.h"

class Renderer {
private:
    std::shared_ptr<Camera> main_camera;
    std::shared_ptr<Shader> default_shader;

    std::unique_ptr<VisualGrid> main_grid;

    std::unique_ptr<VisualLine> main_x_line;
    std::unique_ptr<VisualLine> main_y_line;
    std::unique_ptr<VisualLine> main_z_line;

    std::vector<VisualCube> racket_cubes;

    int racket_render_mode = GL_TRIANGLES;
    glm::vec3 racket_position;
    glm::vec3 racket_rotation;
    glm::vec3 racket_scale;

public:
    Renderer(int _initialWidth, int _initialHeight);

    void Render(GLFWwindow* _window, double _deltaTime);
    void ResizeCallback(GLFWwindow* _window, int _displayWidth, int _displayHeight);
    void InputCallback(GLFWwindow* _window, double _deltaTime);
};

