#include "Renderer.h"
#include "Utility/Input.hpp"
#include "Utility/Transform.hpp"
#include "Texture.h"

Renderer::Renderer(int _initialWidth, int _initialHeight) {
    viewport_width = _initialWidth;
    viewport_height = _initialHeight;

    main_camera = std::make_unique<Camera>(glm::vec3(8.0f, 8.0f, 8.0f), glm::vec3(0.0f), _initialWidth, _initialHeight);

    lights = std::make_shared<std::vector<Light>>();
    lights->emplace_back(glm::vec3(2.0f, 14.0f, 2.0f), glm::vec3(0.99f, 0.95f, 0.78f), 0.1f, 0.4f, 50.0f, 50.0f, Light::Type::POINT);
    lights->emplace_back(glm::vec3(30.0f, 10.0f, 0.0f), glm::vec3(0.09f, 0.95f, 0.08f), 0.2f, 0.4f, 300.0f, 50.0f, Light::Type::SPOT);
    lights->emplace_back(glm::vec3(-30.0f, 10.0f, 0.0f), glm::vec3(0.99f, 0.05f, 0.08f), 0.2f, 0.4f, 300.0f, 50.0f, Light::Type::SPOT);
    lights->emplace_back(glm::vec3(0.0f, 34.0f, 36.0f), glm::vec3(0.09f, 0.05f, 0.78f), 0.2f, 0.4f, 400.0f, 40.0f, Light::Type::SPOT);

    auto grid_shader = Shader::Library::CreateShader("shaders/grid/grid.vert", "shaders/grid/grid.frag");
    auto lit_shader = Shader::Library::CreateShader("shaders/lit/lit.vert", "shaders/lit/lit.frag");
    auto unlit_shader = Shader::Library::CreateShader("shaders/unlit/unlit.vert", "shaders/unlit/unlit.frag");
    auto line_shader = Shader::Library::CreateShader("shaders/line/line.vert", "shaders/line/line.frag");

    auto shadow_mapper_shader = Shader::Library::CreateShader("shaders/shadows/shadow_mapper.vert", "shaders/shadows/shadow_mapper.frag");

    shadow_mapper_material = std::make_unique<Shader::Material>();
    shadow_mapper_material->shader = shadow_mapper_shader;

    Shader::Material main_light_cube_material = {
            .shader = unlit_shader,
            .lights = lights
    };
    main_light_cube = std::make_unique<VisualCube>(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), main_light_cube_material);

    //test cube
    Shader::Material test_material = {
            .shader = lit_shader,
            .color = glm::vec3(0.2f, 0.7f, 0.85f),
            .lights = lights
    };

    test_cube = std::make_unique<VisualCube>(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), test_material);

    //grid
    Shader::Material grid_material = {
            .shader = grid_shader,
            .alpha = 0.4f
    };
    main_grid = std::make_unique<VisualGrid>(100, 100, 1.0f, glm::vec3(0.0f), glm::vec3(90.0f, 0.0f, 0.0f), grid_material);

    //axis lines
    Shader::Material x_line_material = {
            .shader = line_shader,
            .line_thickness = 3.0f,
            .color = glm::vec3(1.0f, 0.0f, 0.0f)
    };

    Shader::Material y_line_material =  {
            .shader = line_shader,
            .line_thickness = 3.0f,
            .color = glm::vec3(0.0f, 1.0f, 0.0f)
    };

    Shader::Material z_line_material =  {
            .shader = line_shader,
            .line_thickness = 3.0f,
            .color = glm::vec3(0.0f, 0.0f, 1.0f)
    };

    //this is a quick way to make the axis lines avoid having depth fighting issues
    main_x_line = std::make_unique<VisualLine>(glm::vec3(0.01f), glm::vec3(5.01f, 0.01f, 0.01f), x_line_material);
    main_y_line = std::make_unique<VisualLine>(glm::vec3(0.01f), glm::vec3(0.01f, 5.01f, 0.01f), y_line_material);
    main_z_line = std::make_unique<VisualLine>(glm::vec3(0.01f), glm::vec3(0.01f, 0.01f, 5.01f), z_line_material);
}

void Renderer::Init() {
    // initializes the shadow map framebuffer
    glGenFramebuffers(1, &shadow_map_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);

    // initializes the shadow map depth texture
    glGenTextures(1, &shadow_map_texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadow_map_texture);

    // sets the texture parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // sets the border color to white, so that the shadow map is white outside the light's view (i.e. no shadow)
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

    // the following 2 blocks mitigate shadow map artifacts, coming from: https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
    // when sampling outside, we don't want a repeating pattern
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT32, Light::LIGHTMAP_SIZE, Light::LIGHTMAP_SIZE, (GLint)lights->size());

    // binds the shadow map depth texture to the framebuffer
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_map_texture, 0);

    // disable color draw & read buffer for this framebuffer
    glReadBuffer(GL_NONE);
    glDrawBuffer(GL_NONE);

    // checks if the framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "ERROR -> Framebuffer is not complete! (" << glCheckFramebufferStatus(GL_FRAMEBUFFER) << "): ";

        switch (glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
            case GL_FRAMEBUFFER_UNDEFINED:
                std::cerr << "GL_FRAMEBUFFER_UNDEFINED" << std::endl;
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << std::endl;
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << std::endl;
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                std::cerr << "GL_FRAMEBUFFER_UNSUPPORTED" << std::endl;
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" << std::endl;
                break;
            default:
                std::cerr << "Unknown error" << std::endl;
                break;
        }
    }

    // cleanup the framebuffer bind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::Render(GLFWwindow* _window, const double _deltaTime) {
    // processes input
    InputCallback(_window, _deltaTime);

    // moves the main light
    auto light_turning_radius = 4.0f;
    for (int i = 1; i < lights->size(); ++i) {
        auto& light = lights->at(i);

        light.SetPosition(glm::vec3(glm::cos(glfwGetTime() * 2.0f + i) * light_turning_radius, 3.0f * glm::sin(glfwGetTime() / 2.0f + i) + 5.0f, glm::sin(glfwGetTime() + i) *  light_turning_radius));
    }

    // SHADOW MAP PASS

    // binds the shadow map framebuffer and the depth texture to draw on it
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
    glViewport(0, 0, Light::LIGHTMAP_SIZE, Light::LIGHTMAP_SIZE);

    glm::mat4 first_world_transform_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f));
    first_world_transform_matrix = glm::scale(first_world_transform_matrix, glm::vec3(2.0f));

    glm::mat4 second_world_transform_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
    second_world_transform_matrix = glm::translate(second_world_transform_matrix, glm::vec3(0.0f, 2.0f, 0.0f));

    for (int i = 0; i < lights->size(); ++i) {
        const auto& light = lights->at(i);

        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_map_texture, 0, i);

        // clears the depth canvas to black
        glClear(GL_DEPTH_BUFFER_BIT);

        //todo: Draw shadow-caster elements HERE (i.e. the cubes below)

        test_cube->DrawFromMatrix(light.GetViewProjection(), light.GetPosition(), first_world_transform_matrix, GL_TRIANGLES, shadow_mapper_material.get());

        test_cube->DrawFromMatrix(light.GetViewProjection(), light.GetPosition(), second_world_transform_matrix, GL_TRIANGLES, shadow_mapper_material.get());
    }

    // unbind the shadow map texture & framebuffer
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // COLOR PASS

    // resets the viewport to the window size
    glViewport(0, 0, viewport_width, viewport_height);

    // activates the shadow map depth texture & binds it to the second texture unit, so that it can be used
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadow_map_texture);

    // clears the color & depth canvas to black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draws the main light cube
    for (const auto& light: *lights) {
        main_light_cube->position = light.GetPosition();
        main_light_cube->material.color = light.GetColor();
        main_light_cube->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());
    }

    // draws the main grid
    main_grid->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());

    // draws the coordinate axis
    main_x_line->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());
    main_y_line->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());
    main_z_line->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());

    //todo: Draw colored elements HERE (i.e. the cubes below)

    test_cube->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), first_world_transform_matrix);

    test_cube->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), second_world_transform_matrix);

    // can be used for post-processing effects
    //main_screen->Draw();
}

void Renderer::ResizeCallback(GLFWwindow* _window, int _displayWidth, int _displayHeight) {
    main_camera->SetViewportSize((float)_displayWidth, (float)_displayHeight);
}

void Renderer::InputCallback(GLFWwindow* _window, const double _deltaTime) {
    //keyboard triggers
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