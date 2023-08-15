#include <random>
#include "Renderer.h"
#include "Utility/Input.hpp"
#include "Utility/Graphics.hpp"
#include "Texture.h"
#include "Utility/Transform.hpp"

Renderer::Renderer(int _initialWidth, int _initialHeight) {
    viewport_width = _initialWidth;
    viewport_height = _initialHeight;

    main_camera = std::make_unique<Camera>(glm::vec3(8.0f, 8.0f, 8.0f), glm::vec3(0.0f), _initialWidth, _initialHeight);

    lights = std::make_shared<std::vector<Light>>();
    lights->emplace_back(glm::vec3(2.0f, 100.0f, 2.0f), glm::vec3(1.0f), 0.1f, 0.2f, 0.4f, Light::Type::DIRECTIONAL);
    //lights->emplace_back(glm::vec3(30.0f, 10.0f, 0.0f), glm::vec3(0.09f, 0.95f, 0.08f), 0.2f, 1.0f, 0.4f, Light::Type::SPOT);
    //lights->emplace_back(glm::vec3(-30.0f, 10.0f, 0.0f), glm::vec3(0.99f, 0.05f, 0.08f), 0.2f, 1.0f, 0.4f, Light::Type::SPOT);
    //lights->emplace_back(glm::vec3(0.0f, 34.0f, 36.0f), glm::vec3(0.09f, 0.05f, 0.78f), 0.2f, 1.0f, 0.4f, Light::Type::SPOT);

    auto lit_shader = Shader::Library::CreateShader("shaders/lit/lit.vert", "shaders/lit/lit.frag");
    auto terrain_lit_shader = Shader::Library::CreateShader("shaders/terrain/lit.vert", "shaders/terrain/lit.frag");
    auto unlit_shader = Shader::Library::CreateShader("shaders/unlit/unlit.vert", "shaders/unlit/unlit.frag");
    auto ocean_shader = Shader::Library::CreateShader("shaders/ocean/ocean.vert", "shaders/ocean/ocean.frag");

    auto screen_shader = Shader::Library::CreateShader("shaders/screen/screen.vert", "shaders/screen/screen.frag");
    auto shadow_mapper_shader = Shader::Library::CreateShader("shaders/shadows/shadow_mapper.vert", "shaders/shadows/shadow_mapper.frag");

    shadow_mapper_material = std::make_unique<Shader::Material>();
    shadow_mapper_material->shader = shadow_mapper_shader;

    Shader::Material screen_material = {
            .shader = screen_shader,
            .lights = lights
    };
    main_screen = std::make_unique<Screen>(viewport_width, viewport_height, 15, screen_material);

    Shader::Material main_light_cube_material = {
            .shader = unlit_shader,
            .lights = lights
    };
    main_light_cube = std::make_unique<VisualCube>(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), main_light_cube_material);

    //grid
    Shader::Material grid_material = {
            .shader = unlit_shader,
            .alpha = 0.4f
    };
    main_grid = std::make_unique<VisualGrid>(100, 100, 1.0f, glm::vec3(0.0f), glm::vec3(90.0f, 0.0f, 0.0f), grid_material);

    //axis lines
    Shader::Material x_line_material = {
            .shader = unlit_shader,
            .line_thickness = 3.0f,
            .color = glm::vec3(1.0f, 0.0f, 0.0f)
    };

    Shader::Material y_line_material =  {
            .shader = unlit_shader,
            .line_thickness = 3.0f,
            .color = glm::vec3(0.0f, 1.0f, 0.0f)
    };

    Shader::Material z_line_material =  {
            .shader = unlit_shader,
            .line_thickness = 3.0f,
            .color = glm::vec3(0.0f, 0.0f, 1.0f)
    };

    //this is a quick way to make the axis lines avoid having depth fighting issues
    main_x_line = std::make_unique<VisualLine>(glm::vec3(0.01f), glm::vec3(5.01f, 0.01f, 0.01f), x_line_material);
    main_y_line = std::make_unique<VisualLine>(glm::vec3(0.01f), glm::vec3(0.01f, 5.01f, 0.01f), y_line_material);
    main_z_line = std::make_unique<VisualLine>(glm::vec3(0.01f), glm::vec3(0.01f, 0.01f, 5.01f), z_line_material);

    //test cube
    Shader::Material test_material = {
            .shader = lit_shader,
            .color = glm::vec3(0.2f, 0.7f, 0.85f),
            .lights = lights
    };

    test_cube = std::make_unique<VisualCube>(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), test_material);

    // Define terrain parameters
    int grid_size = 8;
    float iso_surface_level = 0.0f;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribution(1, 1000);
    int seed = distribution(gen);

    // FOR GETTING XZ RELATED Y WE CAN JUST HAVE SMALL CHUNKS , CHECK WHAT CHUNK WERE IN AND LOOP THROUGH VERTICES OF THAT CHUNK

    Shader::Material terrain_material = {
            .shader = terrain_lit_shader,
            .color = glm::vec3(1.0f),
            .lights = lights
    };

    // Create the terrain generator
    main_terrain = std::make_unique<GenerateTerrain>(grid_size, iso_surface_level, glm::vec3(0.0f,0.0f,0.0f), seed, terrain_material);
    //main_terrain->GenerateChunkTerrain();
    //main_terrain->SetupBuffers();

    //leaf
    Shader::Material Leaf_material =  {
            .shader = lit_shader,
            .line_thickness = 3.0f,
            .lights = lights,
            //.color = glm::vec3(0.0f, 0.0f, 1.0f),
            // .texture = Texture::Library::CreateTexture("assets/fuzz.jpg"),
            // .texture_tiling = glm::vec2(1.0f)
    };

    leaf_cube = std::make_unique<Leaf>(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), Leaf_material);

    //make a stem
    Shader::Material Stem_material =  {
            .shader = lit_shader,
            .line_thickness = 3.0f,
            .lights = lights,
            //.color = glm::vec3(0.0f, 0.0f, 1.0f),
            // .texture = Texture::Library::CreateTexture("assets/clay_texture.jpg"),
            // .texture_tiling = glm::vec2(2.0f)
    };

    stem_cube = std::make_unique<VisualCube>(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), Stem_material);


    //clam shell
    Shader::Material Clam_material =  {
            .shader = lit_shader,
            .line_thickness = 3.0f,
            .color = glm::vec3(0.9f, 0.8f, 0.7f),
            .lights = lights,
    };

    clam_cube = std::make_unique<VisualCube>(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), Clam_material);

    //clam shell
    Shader::Material Pearl_material =  {
            .shader = lit_shader,
            .line_thickness = 3.0f,
            .color = glm::vec3(1.0f, 1.0f, 1.0f),
            .lights = lights,
    };

    pearl_cube = std::make_unique<VisualCube>(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), Pearl_material);

    //clam shell
    Shader::Material lip_material =  {
            .shader = lit_shader,
            .line_thickness = 3.0f,
            .color = glm::vec3(1.0f, 1.0f, 1.0f),
            .lights = lights,
    };

    lip_cube = std::make_unique<VisualCube>(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), lip_material);

    //coral stem
    Shader::Material coral_material =  {
            .shader = lit_shader,
            .line_thickness = 3.0f,
            .color = glm::vec3(1.0f, 1.0f, 1.0f),
            .lights = lights,
    };
    //for internal variation of color
    coral_cube = std::make_unique<VisualCube>(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), coral_material);

    Shader::Material coral_material_2 =  {
            .shader = lit_shader,
            .line_thickness = 3.0f,
            .color = glm::vec3(1.0f, 1.0f, 1.0f),
            .lights = lights,
    };

    coral_cube_2 = std::make_unique<VisualCube>(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), coral_material_2);
    coral_cube_3 = std::make_unique<VisualCube>(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), coral_material_2);

    //ocean
    ocean_material =  std::make_unique<Shader::Material>();
    ocean_material->shader = ocean_shader;
    ocean_material->lights = lights;
    ocean_material->line_thickness = 3.0f;
    ocean_material->color = glm::vec3(0.0f, 0.0f, 1.0f);

    //ocean flow map
    ocean_flow_map = Texture::Library::CreateTexture("assets/textures/flowMap.png");
    main_ocean = std::make_unique<VisualModel>("assets/models/ocean.obj", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), *ocean_material);

    //skybox
    main_skybox = Texture::Library::CreateCubemapTexture("assets/textures/skybox");

    //shark
    shark_material =  std::make_unique<Shader::Material>();
    shark_material->shader = unlit_shader;
    shark_material->lights = lights;
    shark_material->line_thickness = 3.0f;

    shark = std::make_unique<Shark>(100.0f);
    fish = std::vector<Fish>();
    for (int i = 0; i < 10; ++i) {
        fish.emplace_back(50.0f);
    }
}

void Renderer::Init() {
    // SHADOW MAP FRAMEBUFFER

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

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32, Light::LIGHTMAP_SIZE, Light::LIGHTMAP_SIZE, (GLint)lights->size(), 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 nullptr);

    // binds the shadow map depth texture to the framebuffer
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_map_texture, 0);

    // disable color draw & read buffer for this framebuffer
    glReadBuffer(GL_NONE);
    glDrawBuffer(GL_NONE);

    Graphics::ValidateFramebufferStatus();

    // SCREEN FRAMEBUFFER
    main_screen->Init();
}

void Renderer::Render(GLFWwindow* _window, const double _deltaTime) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    const auto current_time = (float)glfwGetTime();

    // processes input
    InputCallback(_window, _deltaTime);

    // resets the viewport to the window size
    glViewport(0, 0, viewport_width, viewport_height);

    // clears the color & depth canvas to black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    main_skybox->UseCubemap(GL_TEXTURE1);
    ocean_flow_map->UseSingle(GL_TEXTURE2);

    ocean_material->shader->SetTexture("skybox", 1);
    ocean_material->shader->SetTexture("oceanFlowMap", 2);

    main_ocean->Draw(main_camera->GetViewProjection(), main_camera->GetPosition(), current_time, GL_TRIANGLES, ocean_material.get());
    shark->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)), current_time, GL_TRIANGLES, shark_material.get());

    for (auto& one_fish : fish) {
        one_fish.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)), current_time, GL_TRIANGLES, shark_material.get());
    }

    Texture::Clear();

    return;

    //animation
    moving_angle = glfwGetTime() * 20.0f;

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

    glm::mat4 second_world_transform_matrix = glm::translate(glm::mat4(1.0f), main_camera->GetPosition());
    second_world_transform_matrix =  glm::scale(second_world_transform_matrix, glm::vec3(500.0f));

    glm::mat4 third_world_transform_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 0.0f));
    third_world_transform_matrix =  glm::scale(third_world_transform_matrix, glm::vec3(1.5f));

    for (int i = 0; i < lights->size(); ++i) {
        const auto& light = lights->at(i);

        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_map_texture, 0, i);

        // clears the depth canvas to black
        glClear(GL_DEPTH_BUFFER_BIT);

        //todo: Draw shadow-caster elements HERE (i.e. the cubes below)

        //test_cube->DrawFromMatrix(light.GetViewProjection(), light.GetPosition(), first_world_transform_matrix, GL_TRIANGLES, shadow_mapper_material.get());

        test_cube->DrawFromMatrix(light.GetViewProjection(), light.GetPosition(), second_world_transform_matrix, current_time, GL_TRIANGLES, shadow_mapper_material.get());

        main_terrain->DrawChunk(light.GetViewProjection(), light.GetPosition(), glm::mat4(1.0f), current_time, GL_TRIANGLES, shadow_mapper_material.get());

        DrawOneWeed(glm::vec3(1.0f,1.0f,1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), light.GetViewProjection(), light.GetPosition(), shadow_mapper_material.get(), moving_angle);
        DrawOneWeed(glm::vec3(2.0f,1.0f,1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), light.GetViewProjection(), light.GetPosition(), shadow_mapper_material.get(), moving_angle);
        DrawOneWeed(glm::vec3(3.0f,1.0f,1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), light.GetViewProjection(), light.GetPosition(), shadow_mapper_material.get(), moving_angle);
        DrawOneWeed(glm::vec3(4.0f,1.0f,1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), light.GetViewProjection(), light.GetPosition(), shadow_mapper_material.get(), moving_angle);


        DrawOneWeed2(glm::vec3(1.0f,1.0f,4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), light.GetViewProjection(), light.GetPosition(), shadow_mapper_material.get(), moving_angle);
        DrawOneClam(glm::vec3(3.0f,1.0f,4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), light.GetViewProjection(), light.GetPosition(), shadow_mapper_material.get(), moving_angle);
        //DrawOneLeaf(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(100.0f), light.GetViewProjection(), light.GetPosition(), shadow_mapper_material.get());
    }

    // COLOR PASS

    // binds the screen to draw on it
    main_screen->Bind();

    // activates the shadow map depth texture & binds it to the second texture unit, so that it can be used for shadow mapping
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadow_map_texture);

    //todo: Draw colored elements HERE (i.e. the cubes, light cubes, grid, etc. below)
    {
        // draws the main light cube
        for (const auto &light: *lights) {
            main_light_cube->position = light.GetPosition();
            main_light_cube->material.color = light.GetColor();
            main_light_cube->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());
        }

        // draws the main grid
        //main_grid->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());

        // draws the coordinate axis
        main_x_line->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());
        main_y_line->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());
        main_z_line->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());

        //test_cube->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), first_world_transform_matrix);

        test_cube->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), second_world_transform_matrix, current_time);

        main_terrain->DrawChunk(main_camera->GetViewProjection(), main_camera->GetPosition(), glm::mat4(1.0f), current_time);

        //DrawOneLeaf(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(10.0f), light.GetViewProjection(), light.GetPosition(), shadow_mapper_material.get());

        DrawOneWeed(glm::vec3(1.0f,1.0f,1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), main_camera->GetViewProjection(), main_camera->GetPosition(), nullptr, moving_angle);
        DrawOneWeed(glm::vec3(2.0f,1.0f,1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), main_camera->GetViewProjection(), main_camera->GetPosition(), nullptr, moving_angle);
        DrawOneWeed(glm::vec3(3.0f,1.0f,1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), main_camera->GetViewProjection(), main_camera->GetPosition(), nullptr, moving_angle);
        DrawOneWeed(glm::vec3(4.0f,1.0f,1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), main_camera->GetViewProjection(), main_camera->GetPosition(), nullptr, moving_angle);

        DrawOneWeed2(glm::vec3(1.0f,1.0f,4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), main_camera->GetViewProjection(), main_camera->GetPosition(), nullptr, moving_angle);

        DrawOneClam(glm::vec3(3.0f,1.0f,4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), main_camera->GetViewProjection(), main_camera->GetPosition(), nullptr, moving_angle);

        DrawOneCoral(glm::vec3(6.0f,1.0f,4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), main_camera->GetViewProjection(), main_camera->GetPosition(), nullptr, moving_angle);

        DrawOneCoral2(glm::vec3(8.0f,1.0f,1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), main_camera->GetViewProjection(), main_camera->GetPosition(), nullptr, moving_angle);
    }

    // unbinds the main screen, so that it can be used as a texture
    main_screen->Unbind();

    // DEFAULT PASS
    {
        // resets the viewport to the window size
        glViewport(0, 0, viewport_width, viewport_height);

        // clears the color & depth canvas to black
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // disables depth testing, because there's no need
        glDisable(GL_DEPTH_TEST);

        // used for post-processing effects
        main_screen->Draw(main_camera->GetViewProjection(), main_camera->GetPosition(), current_time);

        // enables depth testing again
        glEnable(GL_DEPTH_TEST);
    }
}

void Renderer::ResizeCallback(GLFWwindow* _window, int _displayWidth, int _displayHeight) {
    // updates the viewport size
    viewport_width = _displayWidth;
    viewport_height = _displayHeight;

    main_camera->SetViewportSize((float)_displayWidth, (float)_displayHeight);
    main_screen->ResizeCallback(_displayWidth, _displayHeight);
}

void Renderer::DrawOneWeed(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection, const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time)
{
    glm::mat4 world_transform_matrix = glm::mat4(1.0f);

    world_transform_matrix = glm::translate(world_transform_matrix, _position);
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, _rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, _scale);

    auto scale_factor = glm::vec3(0.3f, 0.3f, 0.3f);         // scale for one cube


    glm::mat4 secondary_transform_matrix = world_transform_matrix;

    //animation values
    //float moving_angle = glm::cos(glfwGetTime() * 2.0f);

    //base stem
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    //world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    stem_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    //weed color
    glm::vec3 weedcolor = glm::vec3(0.197f, 0.321f, 0.118f);

    float x_offset = 0.05f;
    float y_offset = 0.3f;
    float rot_bounce = 90.0f;
    float bounce = 1.0f;
    float bounce_sin = 1.0f;

    float sin_offset = cos(time/20)/20;

    stem_cube->material.color = weedcolor; // colour


    for(int x = 0; x < 10; x++){

        if(bounce_sin / 3.0f == 1.0f){
            bounce_sin = 1.0f;
            x_offset = -x_offset;
        }

        //draw stem block
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(x_offset+sin_offset, y_offset, 0.0f));
        //world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, bounce * rot_bounce, 0.0f));
        world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
        stem_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, time, GL_TRIANGLES, _materialOverride);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
        //world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, -bounce * rot_bounce, 0.0f));

        //draw leaf block

        // world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, bounce * rot_bounce, 0.0f));
        // DrawOneLeaf(secondary_transform_matrix, _viewProjection, _eyePosition, _materialOverride);
        // secondary_transform_matrix = world_transform_matrix;
        // world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, -bounce * rot_bounce, 0.0f));

        //bounce
        bounce += 1.0f;
        bounce_sin += 1.0f;



    }


}


void Renderer::DrawOneWeed2(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection, const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time)
{
    glm::mat4 world_transform_matrix = glm::mat4(1.0f);

    world_transform_matrix = glm::translate(world_transform_matrix, _position);
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, _rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, _scale);

    auto scale_factor = glm::vec3(0.1f, 0.1f, 0.1f);         // scale for one cube


    glm::mat4 secondary_transform_matrix = world_transform_matrix;

    //animation values
    //float moving_angle = glm::cos(glfwGetTime() * 2.0f);


    //base stem
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    //world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    stem_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    float x_offset = 0.025f;
    float y_offset = 0.1f;
    float rot_bounce = 90.0f;
    float bounce = 1.0f;
    float bounce_sin = 1.0f;
    int counter = 0;

    float sin_offset = cos(time/20)/20;

    //weed color
    glm::vec3 weedcolor = glm::vec3(0.197f, 0.321f, 0.118f);
    stem_cube->material.color = weedcolor; // colour

    //leaf color
    glm::vec3 leafcolor = glm::vec3(1.0f, 0.5f, 0.2f);
    leaf_cube->material.color = leafcolor; // colour



    for(int x = 0; x < 70; x++){

        if(bounce_sin / 2.0f == 1.0f){
            bounce_sin = 1.0f;
            x_offset = -x_offset;
        }

        //draw stem block
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(x_offset, y_offset, 0.0f));
        //world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, bounce * rot_bounce, 0.0f));
        world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
        stem_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, time, GL_TRIANGLES, _materialOverride);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
        //world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, -bounce * rot_bounce, 0.0f));

        //draw leaf block

        if(counter == 5){
            world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, bounce * rot_bounce + sin_offset*25, 0.0f));
            world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 45.0f));

            DrawOneLeaf(secondary_transform_matrix, _viewProjection, _eyePosition, _materialOverride);

            secondary_transform_matrix = world_transform_matrix;
            world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, -45.0f));
            world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, -bounce * rot_bounce + sin_offset*25, 0.0f));
            counter = 0;
        }


        //bounce
        counter += 1;
        bounce += 1.0f;
        bounce_sin += 1.0f;

    }


}


void Renderer::DrawOneClam(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection, const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time)
{
    glm::mat4 world_transform_matrix = glm::mat4(1.0f);

    world_transform_matrix = glm::translate(world_transform_matrix, _position);
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, _rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, _scale);

    auto scale_factor = glm::vec3(0.1f, 0.1f, 0.1f);         // scale for one cube
    auto scale_factor_lip = glm::vec3(0.1f, 0.1f, 0.1f);         // scale for one cube

    float sin_offset = abs(cos(time/60) * 70);

    glm::mat4 secondary_transform_matrix = world_transform_matrix;

    //new clam model
    //make varying heighted strips
    //and add a small lip
    clam_cube->material.color = glm::vec3(106.0f/255.0f, 90.0f/255.0f, 205.0f/255.0f); // colour
    scale_factor = glm::vec3(0.1f, 0.1f, 1.0f);
    //make lip for that strand
    lip_cube->material.color = glm::vec3(255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f); // colour
    scale_factor_lip = glm::vec3(0.1f, 0.1f, 0.1f);



    pearl_cube->material.color = glm::vec3(181.0f/255.0f, 165.0f/255.0f, 213.0f/255.0f); // colour

    //first strand is highest

    //shell
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.15f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor - glm::vec3(0.0f, 0.0f, 0.05f) );
    clam_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / (scale_factor- glm::vec3(0.0f, 0.0f, 0.05f)));
    //lip
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.05f, 0.525f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
    lip_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);

    //start patern

    //shell
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.1f, -0.1f, -0.50f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    clam_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
    //lip
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.05f, 0.55f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
    lip_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);

    //autofill remainder
    for(int i = 1; i<7; i++){

        if( i % 2 == 0){ //if even

            //top strand
            world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.1f, 0.0f, -0.55f));
            world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
            clam_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
            world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
            //lip
            world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.05f, 0.55f));
            world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
            lip_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
            world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);

        }else{
            //bottom strand
            world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.1f, -0.1f, -0.55f));
            world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
            pearl_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
            world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
            //lip
            world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.05f, 0.55f));
            world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
            lip_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
            world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);

        }

    }


    //do last strand 1 step higher

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.1f, 0.0f, -0.60f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor - glm::vec3(0.0f, 0.0f, 0.05f));
    clam_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / (scale_factor - glm::vec3(0.0f, 0.0f, 0.05f)));
    //lip
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.05f, 0.525f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
    lip_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);


    //draw pearl in middle

    //use lip since it is also white

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.4f, 0.0f, -0.45f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.2f, 0.2f, 0.2f));
    lip_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / glm::vec3(0.2f, 0.2f, 0.2f));

    //edges no lip
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.5f, 0.05f, -0.05f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor - glm::vec3(0.0f, 0.0f, 0.05f));
    clam_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / (scale_factor - glm::vec3(0.0f, 0.0f, 0.05f)));

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-1.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor - glm::vec3(0.0f, 0.0f, 0.05f));
    clam_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / (scale_factor - glm::vec3(0.0f, 0.0f, 0.05f)));

    //start top pattern
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.1f, 0.0f, -0.525f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip );
    clam_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip );

    for(int i = 0; i<8; i++){
        //make the back face top
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.1f, 0.0f, 0.0f));
        world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip );
        clam_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip );

    }

    //1 level lower
    //start pattern
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.1f, -0.1f, -0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip );
    clam_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip );

    for(int i = 0; i<6; i++){
        //make the back face top
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.1f, 0.0f, 0.0f));
        world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip );
        clam_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip );

    }


    //now repeat to get the top half

    //go up 2 move to middle make a line
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3( 0.3f, 0.2f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(sin_offset, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3( 0.9f, 0.1f, 0.1f));
    clam_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / glm::vec3( 0.9f, 0.1f, 0.1f) );
    //second back plate
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3( 0.0f, 0.05f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3( 0.7f, 0.05f, 0.1f));
    clam_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / glm::vec3( 0.7f, 0.05f, 0.1f) );

    //set up transition for new top strands
    //go back down 1 and start at the edge
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3( 0.4f, -0.05f, 0.50f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor - glm::vec3(0.0f, 0.0f, 0.05f));
    pearl_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f /( scale_factor- glm::vec3(0.0f, 0.0f, 0.05f)));
    //make lip
    //lip_cube->material.color = glm::vec3(55.0f/255.0f, 25.0f/255.0f, 25.0f/255.0f); // colour
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3( 0.0f, -0.05f, 0.525f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
    lip_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);

    //now go 1 up
    //top strand
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.1f, 0.1f, -0.50f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    clam_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
    //lip
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -0.05f, 0.55f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
    lip_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);

    //up and pearl color
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.1f, 0.1f, -0.55f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    pearl_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
    //lip
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -0.05f, 0.55f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
    lip_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);


    for(int i = 1; i<6; i++){

        if( i % 2 == 0){ //if even

            //top strand
            world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.1f, 0.1f, -0.55f));
            world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
            pearl_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
            world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
            //lip
            world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -0.05f, 0.55f));
            world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
            lip_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
            world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);

        }else{
            //bottom strand
            world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.1f, 0.0f, -0.55f));
            world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
            clam_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
            world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
            //lip
            world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -0.05f, 0.55f));
            world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
            lip_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
            world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);

        }

    }

    //last strand
    //1 setp lower
    //less long
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.1f, 0.0f, -0.55f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor - glm::vec3(0.0f, 0.0f, 0.05f));
    pearl_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / (scale_factor - glm::vec3(0.0f, 0.0f, 0.05f)));
    //lip
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -0.05f, 0.525f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
    lip_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);




}



void Renderer::DrawOneCoral(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection, const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time){


    glm::mat4 world_transform_matrix = glm::mat4(1.0f);

    world_transform_matrix = glm::translate(world_transform_matrix, _position);
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, _rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, _scale);


    float sin_offset = cos(time/60) * 2;

    glm::mat4 secondary_transform_matrix = world_transform_matrix;

    coral_cube_3->material.color = glm::vec3(1.0f, 0.7f, 0.0f); // colour --should be the darker one

    coral_cube->material.color = glm::vec3(1.0f, 0.8f, 0.0f); // colour --should be the darker one
    coral_cube_2->material.color = glm::vec3(1.0f, 1.0f, 0.0f); // colour for later gradient -- should be the lighter one
    //lip_cube->material.color = glm::vec3(255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f); // colour


    //special values

    float height = 3.0f;
    float angle_side = 45.0f;
    float angle_upward = 30.0f;
    float bounce = 1.0f;
    float nb_branches = 20.0f;

    auto scale_factor = glm::vec3(0.1f, height/1.75, 0.1f);         // scale for pilar cube
    auto scale_factor_tier2 = glm::vec3(0.1f, height/2, 0.1f);     // scale for one cube it the second hierachy
    auto scale_factor_tier3 = glm::vec3(0.05f, height/2.25, 0.05f);     // scale for one cube it the second hierachy

    float increment = height / nb_branches;
    float circle = 360.0f / nb_branches;
    float radius = 2.0f;
    //reset position
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -height/2, 0.0f));


    for(int i =0; nb_branches*1.5 > i; i++ ){
        //every branch will have 1+ tier 3 branches


        world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,360.0f/(nb_branches*1.5), 0.0f));
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(radius/1.25, height/4, 0.0f));
        world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,sin_offset, 35.0f));
        world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_tier3 );
        coral_cube_2->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_tier3);
        world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,-sin_offset, -35.0f));
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-radius/1.25, -height/4, 0.0f));

    }

    for(int i =0; nb_branches > i; i++ ){
        //every branch will have 1+ tier 3 branches


        world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,circle, 0.0f));
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(radius/1.5, height/4, 0.0f));
        world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,0.0f, 35.0f));
        world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_tier2 );
        coral_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_tier2);
        world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,0.0f, -35.0f));
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-radius/1.5, -height/4, 0.0f));

    }

    for(int i =0; nb_branches > i; i++ ){
        //every branch will have 1+ tier 3 branches


        world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,circle, 0.0f));
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(radius/2, height/4, 0.0f));
        world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,0.0f, 25.0f));
        world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_tier2 );
        coral_cube_2->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_tier2);
        world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,0.0f, -25.0f));
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-radius/2, -height/4, 0.0f));

    }



    for(int i =0; nb_branches > i; i++ ){
        //every branch will have 1+ tier 3 branches


        world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,circle, 0.0f));
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(radius/3, height/3, 0.0f));
        world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(sin_offset,0.0f, 15.0f));
        world_transform_matrix = glm::scale(world_transform_matrix, scale_factor );
        coral_cube_3->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
        world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(-sin_offset,0.0f, -15.0f));
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-radius/3, -height/3, 0.0f));

    }


}


void Renderer::DrawOneCoral2(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection, const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time){


    glm::mat4 world_transform_matrix = glm::mat4(1.0f);

    world_transform_matrix = glm::translate(world_transform_matrix, _position);
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, _rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, _scale);


    float sin_offset = cos(time/30) * 2;

    glm::mat4 secondary_transform_matrix = world_transform_matrix;

    coral_cube_3->material.color = glm::vec3(1.0f, 0.7f, 0.0f); // colour --should be the darker one

    coral_cube->material.color = glm::vec3(1.0f, 0.8f, 0.0f); // colour --should be the darker one
    coral_cube_2->material.color = glm::vec3(1.0f, 1.0f, 0.0f); // colour for later gradient -- should be the lighter one
    //lip_cube->material.color = glm::vec3(255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f); // colour


    //special values
    float nb_branches = 5.0f;
    float height = 3.0f;
    float angle_side = 360.0f/nb_branches;
    float angle_upward = 30.0f;
    float bounce = 1.0f;
    float angle_upward2 ;


    auto scale_factor = glm::vec3(0.1f, height, 0.1f);         // scale for pilar cube
    auto scale_factor_tier2 = glm::vec3(0.1f, height/2, 0.1f);     // scale for one cube it the second hierachy
    auto scale_factor_tier3 = glm::vec3(0.05f, height/4, 0.05f);     // scale for one cube it the second hierachy

    float increment = height / nb_branches;
    // float circle = 360.0f / nb_branches;
    // float radius = 2.0f;
    //reset position
    //world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -height/2, 0.0f));

    //main pilar
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, height/2, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor );
    coral_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -height/2, 0.0f));



    for(int i =0; nb_branches-1 > i; i++ ){
        //every branch will have 1+ tier 3 branches

        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, increment, 0.0f));
        world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,sin_offset/2, angle_upward));
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, height/4, 0.0f));
        world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_tier2 );
        coral_cube_2->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_tier2);

        //make a tier 3 branch

        angle_upward2 = angle_upward + i*3;

        world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,sin_offset, angle_upward2));
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, height/8, 0.0f));
        world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_tier3 );
        coral_cube_3->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_tier3);
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -height/8, 0.0f));
        world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,-sin_offset, -angle_upward2));

        //return to pillar


        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -height/4, 0.0f));
        world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,-sin_offset/2, -angle_upward));
        world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,angle_side, 0.0f));

    }

    //last one is 1 down
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -increment, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,0.0f, angle_upward));
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, height/4, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_tier2 );
    coral_cube_2->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_tier2);
    //tier 3
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,0.0f, angle_upward2));
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, height/8, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_tier3 );
    coral_cube_3->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,time, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_tier3);
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -height/8, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,0.0f, -angle_upward2));
    //clear
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -height/4, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,0.0f, -angle_upward));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,angle_side, 0.0f));






}








void Renderer::DrawOneLeaf(glm::mat4 world_transform_matrix, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride){
    auto scale_factor = glm::vec3(1.0f, 1.0f, 1.0f);         // scale for one cube
    // global transforms
    //world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec(0.0f));
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-1.0f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(90.0f, 0.0f, 90.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    leaf_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, 0.0f, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);


}

void Renderer::InputCallback(GLFWwindow* _window, const double _deltaTime) {
    //keyboard triggers
    //camera translates (side to side and zoom forwards & back)
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_7) || Input::IsKeyPressed(_window, GLFW_KEY_Q))
        main_camera->OneAxisMove(Camera::Translation::UP, 0.1f);
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_9) || Input::IsKeyPressed(_window, GLFW_KEY_E))
        main_camera->OneAxisMove(Camera::Translation::DOWN, 0.1f);
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_4) || Input::IsKeyPressed(_window, GLFW_KEY_W))
        main_camera->OneAxisMove(Camera::Translation::FORWARD, 0.1f);
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_6) || Input::IsKeyPressed(_window, GLFW_KEY_A))
        main_camera->OneAxisMove(Camera::Translation::LEFT, 0.1f);
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_8) || Input::IsKeyPressed(_window, GLFW_KEY_S))
        main_camera->OneAxisMove(Camera::Translation::BACKWARD, 0.1f);
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_2) || Input::IsKeyPressed(_window, GLFW_KEY_D))
        main_camera->OneAxisMove(Camera::Translation::RIGHT, 0.1f);

    //mouse triggers
    //forwards & back in camera local coordinates
    if (Input::IsMouseButtonPressed(_window, GLFW_MOUSE_BUTTON_LEFT))
        main_camera->OneAxisMove(Camera::Translation::CAMERA_FORWARD, (float)Input::cursor_delta_y / 10.0f);

    //tilt control
    if (Input::IsMouseButtonPressed(_window, GLFW_MOUSE_BUTTON_MIDDLE))
        main_camera->OneAxisRotate(Camera::Rotation::POSITIVE_PITCH, (float)Input::cursor_delta_y / 10.0f);

    //pan control
    if (Input::IsMouseButtonPressed(_window, GLFW_MOUSE_BUTTON_RIGHT))
        main_camera->OneAxisRotate(Camera::Rotation::POSITIVE_YAW, (float)Input::cursor_delta_x / 10.0f);

    //camera rotation reset
    if (Input::IsKeyPressed(_window, GLFW_KEY_HOME) || Input::IsKeyPressed(_window, GLFW_KEY_KP_5))
        main_camera->SetDefaultPositionAndTarget();

    //keyboard triggers
    //camera orbit
    if (Input::IsKeyPressed(_window, GLFW_KEY_UP))
        main_camera->OneAxisOrbit(Camera::Orbitation::ORBIT_UP, 0.1f);
    if (Input::IsKeyPressed(_window, GLFW_KEY_DOWN))
        main_camera->OneAxisOrbit(Camera::Orbitation::ORBIT_DOWN, 0.1f);
    if (Input::IsKeyPressed(_window, GLFW_KEY_RIGHT))
        main_camera->OneAxisOrbit(Camera::Orbitation::ORBIT_RIGHT, 0.1f);
    if (Input::IsKeyPressed(_window, GLFW_KEY_LEFT))
        main_camera->OneAxisOrbit(Camera::Orbitation::ORBIT_LEFT, 0.1f);
}