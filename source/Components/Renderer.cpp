#include "Renderer.h"
#include "Utility/Input.hpp"
#include "Utility/Transform.hpp"

Renderer::Renderer(int _initialWidth, int _initialHeight) {
    main_camera = std::make_unique<Camera>(glm::vec3(13.0f, 13.0f, 21.0f), glm::vec3(0.0f), _initialWidth, _initialHeight);

    default_shader = Shader::Library::CreateShader("shaders/default.vert", "shaders/default.frag");

    auto main_light_position = glm::vec3(10.0f, 0.0f, 13.0f);
    auto main_light_color = glm::vec3(0.99f, 0.95f, 0.78f);

    //grid
    Shader::Descriptor grid_s_descriptor = {
            .vertex_shader_path = "shaders/grid/grid.vert",
            .fragment_shader_path = "shaders/grid/grid.frag",
            .alpha = 0.4f
    };
    main_grid = std::make_unique<VisualGrid>(100, 100, 100.0f, glm::vec3(0.0f), glm::vec3(0.0f), grid_s_descriptor);

    //axis lines
    const char* line_vertex_shader_path = "shaders/line/line.vert";
    const char* line_fragment_shader_path = "shaders/line/line.frag";

    Shader::Descriptor x_line_s_descriptor = {
            .vertex_shader_path = line_vertex_shader_path,
            .fragment_shader_path = line_fragment_shader_path,
            .line_thickness = 3.0f,
            .color = glm::vec3(1.0f, 0.0f, 0.0f)
    };

    Shader::Descriptor y_line_s_descriptor =  {
            .vertex_shader_path = line_vertex_shader_path,
            .fragment_shader_path = line_fragment_shader_path,
            .line_thickness = 3.0f,
            .color = glm::vec3(0.0f, 1.0f, 0.0f)
    };

    Shader::Descriptor z_line_s_descriptor =  {
            .vertex_shader_path = line_vertex_shader_path,
            .fragment_shader_path = line_fragment_shader_path,
            .line_thickness = 3.0f,
            .color = glm::vec3(0.0f, 0.0f, 1.0f)
    };

    //this is a quick way to make the axis lines avoid having depth fighting issues
    main_x_line = std::make_unique<VisualLine>(glm::vec3(0.01f), glm::vec3(5.01f, 0.01f, 0.01f), x_line_s_descriptor);
    main_y_line = std::make_unique<VisualLine>(glm::vec3(0.01f), glm::vec3(0.01f, 5.01f, 0.01f), y_line_s_descriptor);
    main_z_line = std::make_unique<VisualLine>(glm::vec3(0.01f), glm::vec3(0.01f, 0.01f, 5.01f), z_line_s_descriptor);
}

void Renderer::Render(GLFWwindow* _window, const double _deltaTime) {
    //clears the canvas to black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //processes keyboard input
    InputCallback(_window, _deltaTime);

    //activates the default shader
    default_shader->Use();

    //draws the main grid
    main_grid->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());

    //draws the coordinate axis
    main_x_line->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());
    main_y_line->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());
    main_z_line->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());
}

void Renderer::ResizeCallback(GLFWwindow* _window, int _displayWidth, int _displayHeight) {
    main_camera->SetViewportSize((float)_displayWidth, (float)_displayHeight);
}

void Renderer::InputCallback(GLFWwindow* _window, const double _deltaTime) {
    //keyboard triggers
    //render mode
    if (Input::IsKeyPressed(_window, GLFW_KEY_P))
        racket_render_mode = GL_POINTS;
    else if (Input::IsKeyPressed(_window, GLFW_KEY_L))
        racket_render_mode = GL_LINE_LOOP;
    else if (Input::IsKeyPressed(_window, GLFW_KEY_T))
        racket_render_mode = GL_TRIANGLES;

    //model transforms
    //translation
    if (Input::IsKeyReleased(_window, GLFW_KEY_SPACE)) //I know that std::rand isn't the best, but it'll do
        racket_position = glm::vec3(-50.0f + std::rand() / (float)RAND_MAX * 100.0f, -50.0f + std::rand() / (float)RAND_MAX * 100.0f, 0.0f);
    else if (Input::IsKeyReleased(_window, GLFW_KEY_TAB)) {
        racket_rotation = racket_position = glm::vec3(0.0f);
        racket_scale = glm::vec3(1.0f);
    }

    if (Input::IsKeyPressed(_window, GLFW_KEY_W) && Input::IsKeyPressed(_window, GLFW_KEY_LEFT_SHIFT))
        racket_position += glm::vec3(-1.0f, 0.0f, 0.0f) * (float)_deltaTime;
    if (Input::IsKeyPressed(_window, GLFW_KEY_S) && Input::IsKeyPressed(_window, GLFW_KEY_LEFT_SHIFT))
        racket_position += glm::vec3(1.0f, 0.0f, 0.0f) * (float)_deltaTime;
    if (Input::IsKeyPressed(_window, GLFW_KEY_A) && Input::IsKeyPressed(_window, GLFW_KEY_LEFT_SHIFT))
        racket_position += glm::vec3(0.0f, -1.0f, 0.0f) * (float)_deltaTime;
    if (Input::IsKeyPressed(_window, GLFW_KEY_D) && Input::IsKeyPressed(_window, GLFW_KEY_LEFT_SHIFT))
        racket_position += glm::vec3(0.0f, 1.0f, 0.0f) * (float)_deltaTime;

    //scale
    if (Input::IsKeyPressed(_window, GLFW_KEY_U))
        racket_scale += glm::vec3(1.0f) * (float)_deltaTime;
    if (Input::IsKeyPressed(_window, GLFW_KEY_J))
        racket_scale += glm::vec3(-1.0f) * (float)_deltaTime;

    //camera translates (side to side and zoom forwards & back)
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_7) || Input::IsKeyPressed(_window, GLFW_KEY_Q))
        main_camera->OneAxisMove(Camera::Translation::UP, (float)_deltaTime);
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_9) || Input::IsKeyPressed(_window, GLFW_KEY_E))
        main_camera->OneAxisMove(Camera::Translation::DOWN, (float)_deltaTime);
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_4) || Input::IsKeyPressed(_window, GLFW_KEY_W))
        main_camera->OneAxisMove(Camera::Translation::FORWARD, (float)_deltaTime);
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_6) || Input::IsKeyPressed(_window, GLFW_KEY_A))
        main_camera->OneAxisMove(Camera::Translation::LEFT, (float)_deltaTime);
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_8) || Input::IsKeyPressed(_window, GLFW_KEY_S))
        main_camera->OneAxisMove(Camera::Translation::BACKWARD, (float)_deltaTime);
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_2) || Input::IsKeyPressed(_window, GLFW_KEY_D))
        main_camera->OneAxisMove(Camera::Translation::RIGHT, (float)_deltaTime);

    //mouse triggers
    //forwards & back in camera local coordinates
    if (Input::IsMouseButtonPressed(_window, GLFW_MOUSE_BUTTON_LEFT))
        main_camera->OneAxisMove(Camera::Translation::CAMERA_FORWARD, (float)(_deltaTime * Input::cursor_delta_y));

    //tilt control
    if (Input::IsMouseButtonPressed(_window, GLFW_MOUSE_BUTTON_MIDDLE))
        main_camera->OneAxisRotate(Camera::Rotation::POSITIVE_PITCH, (float)(_deltaTime * Input::cursor_delta_y));

    //pan control
    if (Input::IsMouseButtonPressed(_window, GLFW_MOUSE_BUTTON_RIGHT))
        main_camera->OneAxisRotate(Camera::Rotation::POSITIVE_YAW, (float) (_deltaTime * Input::cursor_delta_x));

    //camera rotation reset
    if (Input::IsKeyPressed(_window, GLFW_KEY_HOME) || Input::IsKeyPressed(_window, GLFW_KEY_KP_5))
        main_camera->SetDefaultPositionAndTarget();

    //keyboard triggers
    //camera orbit
    if (Input::IsKeyPressed(_window, GLFW_KEY_UP))
        main_camera->OneAxisOrbit(Camera::Orbitation::ORBIT_UP, (float)_deltaTime);
    if (Input::IsKeyPressed(_window, GLFW_KEY_DOWN))
        main_camera->OneAxisOrbit(Camera::Orbitation::ORBIT_DOWN, (float)_deltaTime);
    if (Input::IsKeyPressed(_window, GLFW_KEY_RIGHT))
        main_camera->OneAxisOrbit(Camera::Orbitation::ORBIT_RIGHT, (float)_deltaTime);
    if (Input::IsKeyPressed(_window, GLFW_KEY_LEFT))
        main_camera->OneAxisOrbit(Camera::Orbitation::ORBIT_LEFT, (float)_deltaTime);
}
