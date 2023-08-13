#pragma once

#include <map>
#include "Camera.h"
#include "Shader.h"
#include "Visual/VisualGrid.h"
#include "GLFW/glfw3.h"
#include "Components/Visual/VisualLine.h"
#include "Components/Visual/VisualCube.h"
#include "Components/Visual/Leaf.h"
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

    //plants and seaweed
    std::unique_ptr<Leaf> leaf_cube;
    std::unique_ptr<VisualCube>  stem_cube;
    //clam
    std::unique_ptr<VisualCube>  clam_cube;
    std::unique_ptr<VisualCube>  pearl_cube;
    std::unique_ptr<VisualCube>  lip_cube;
    //coral
    std::unique_ptr<VisualCube>  coral_cube;
    std::unique_ptr<VisualCube>  coral_cube_2;
    std::unique_ptr<VisualCube>  coral_cube_3;

    int viewport_width, viewport_height;

    float moving_angle;

    GLuint shadow_map_fbo = 0;
    GLuint shadow_map_texture = 0;

public:
    Renderer(int _initialWidth, int _initialHeight);

    void Init();
    void Render(GLFWwindow* _window, double _deltaTime);

    void ResizeCallback(GLFWwindow* _window, int _displayWidth, int _displayHeight);
    void InputCallback(GLFWwindow* _window, double _deltaTime);

    void DrawOneWeed(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time);
    void DrawOneWeed2(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time);
    void DrawOneClam(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time);
    void DrawOneCoral(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time);
    void DrawOneCoral2(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time);
    void DrawOneLeaf(glm::mat4 world_transform_matrix, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride);
};

