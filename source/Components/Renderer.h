#pragma once

#include <map>
#include "Camera.h"
#include "Shader.h"
#include "Visual/VisualGrid.h"
#include "GLFW/glfw3.h"
#include "Components/Visual/VisualLine.h"
#include "Components/Visual/VisualCube.h"
#include "Screen.h"

class Renderer {
private:
    std::unique_ptr<Screen> main_screen;
    std::shared_ptr<Camera> main_camera;
    std::unique_ptr<Shader::Material> shadow_mapper_material;

    std::unique_ptr<VisualGrid> main_grid;

    std::unique_ptr<VisualLine> main_x_line;
    std::unique_ptr<VisualLine> main_y_line;
    std::unique_ptr<VisualLine> main_z_line;

    std::shared_ptr<std::vector<Light>> lights;
    std::unique_ptr<VisualCube> main_light_cube;

    std::unique_ptr<VisualCube> test_cube;

    int viewport_width, viewport_height;

    GLuint shadow_map_fbo = 0;
    GLuint shadow_map_texture = 0;

public:
    Renderer(int _initialWidth, int _initialHeight);

    void Init();
    void Render(GLFWwindow* _window, double _deltaTime);

    void ResizeCallback(GLFWwindow* _window, int _displayWidth, int _displayHeight);
    void InputCallback(GLFWwindow* _window, double _deltaTime);
};

