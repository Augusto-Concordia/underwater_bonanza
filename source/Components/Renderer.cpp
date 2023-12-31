#include <random>
#include "Renderer.h"
#include "Utility/Input.hpp"
#include "Utility/Graphics.hpp"
#include "Texture.h"
#include "Utility/Transform.hpp"
#include "SFML/Audio/SoundBuffer.hpp"

Renderer::Renderer(int _initialWidth, int _initialHeight) {
    viewport_width = _initialWidth;
    viewport_height = _initialHeight;

    main_camera = std::make_unique<Camera>(glm::vec3(50.0f, 40.0f, 50.0f), glm::vec3(25.0f , 25.0f, 25.0f), _initialWidth, _initialHeight);

    lights = std::make_shared<std::vector<Light>>();
    lights->emplace_back(glm::vec3(60.0f, 100.0f, 60.0f), glm::vec3(1.0f), 0.2f, 0.2f, 0.4f, Light::Type::DIRECTIONAL);
    lights->emplace_back(glm::vec3(30.0f, 10.0f, 30.0f), glm::vec3(0.09f, 0.95f, 0.08f), 0.2f, 1.0f, 0.4f, Light::Type::SPOT);
    lights->emplace_back(glm::vec3(40.0f, 10.0f, 20.0f), glm::vec3(0.99f, 0.05f, 0.08f), 0.2f, 1.0f, 0.4f, Light::Type::SPOT);
    lights->emplace_back(glm::vec3(20.0f, 12.0f, 40.0f), glm::vec3(0.09f, 0.05f, 0.78f), 0.2f, 1.0f, 0.4f, Light::Type::SPOT);
    lights->emplace_back(glm::vec3(30.0f, 10.0f, 30.0f), glm::vec3(0.09f, 0.95f, 0.08f), 0.2f, 1.0f, 0.4f, Light::Type::SPOT);
    lights->emplace_back(glm::vec3(40.0f, 10.0f, 20.0f), glm::vec3(0.99f, 0.05f, 0.08f), 0.2f, 1.0f, 0.4f, Light::Type::SPOT);
    lights->emplace_back(glm::vec3(20.0f, 12.0f, 40.0f), glm::vec3(0.09f, 0.05f, 0.78f), 0.2f, 1.0f, 0.4f, Light::Type::SPOT);

    lights->at(0).SetRange(600.0f);
    lights->at(0).SetTarget(glm::vec3(50.0f, 0.0f, 50.0f));

    for (int i = 1; i < lights->size(); ++i) {
        lights->at(i).SetRange(25.0f);
    }

    /*lights->at(1).SetTarget(glm::vec3(30.0f, 12.0f, 30.0f));
    lights->at(2).SetTarget(glm::vec3(40.0f, 12.0f, 20.0f));
    lights->at(3).SetTarget(glm::vec3(20.0f, 12.0f, 40.0f));*/

    lit_shader = Shader::Library::CreateShader("shaders/lit/lit.vert", "shaders/lit/lit.frag");
    auto terrain_lit_shader = Shader::Library::CreateShader("shaders/terrain/lit.vert", "shaders/terrain/lit.frag");
    auto unlit_shader = Shader::Library::CreateShader("shaders/unlit/unlit.vert", "shaders/unlit/unlit.frag");
    auto ocean_shader = Shader::Library::CreateShader("shaders/ocean/ocean.vert", "shaders/ocean/ocean.frag");
    auto skybox_shader = Shader::Library::CreateShader("shaders/skybox/skybox.vert", "shaders/skybox/skybox.frag");
    auto model_shader = Shader::Library::CreateShader("shaders/model/model.vert", "shaders/model/model.frag");

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
    //main_grid = std::make_unique<VisualGrid>(100, 100, 1.0f, glm::vec3(0.0f), glm::vec3(90.0f, 0.0f, 0.0f), grid_material);

    Shader::Material titleScreen_Material = {
            .shader = unlit_shader,
            .line_thickness = 3.0f,
            .color = glm::vec3(1.0f, 1.0f, 1.0f),
            .alpha = 0.0f,
            .texture= Texture::Library::CreateTexture("assets/prettyBlue_logo.png"),
            .texture_influence = 1.0f,
    };
    Shader::Material blackFadeUI_Material = {
            .shader = unlit_shader,
            .line_thickness = 3.0f,
            .color = glm::vec3(1.0f, 1.0f, 1.0f),
            .alpha = 0.0f,
            .texture = Texture::Library::CreateTexture("assets/black.png"),
            .texture_influence = 1.0f,
    };
    Shader::Material whiteFadeUI_Material = {
            .shader = unlit_shader,
            .line_thickness = 3.0f,
            .color = glm::vec3(1.0f, 1.0f, 1.0f),
            .alpha = 0.0f,
            .texture = Texture::Library::CreateTexture("assets/white.png"),
            .texture_influence = 1.0f,
    };
    Shader::Material pressSpaceUI_Material = {
            .shader = unlit_shader,
            .line_thickness = 3.0f,
            .color = glm::vec3(1.0f, 1.0f, 1.0f),
            .alpha = 0.0f,
            .texture = Texture::Library::CreateTexture("assets/pressSpace.png"),
            .texture_influence = 1.0f,
    };
   
    titleScreenUI = std::make_unique<VisualPlane>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.5f, 0.5f, 0.5f), titleScreen_Material);
    blackFadeUI = std::make_unique<VisualPlane>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.5f, 0.5f, 0.5f), blackFadeUI_Material);
    whiteFadeUI = std::make_unique<VisualPlane>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.5f, 0.5f, 0.5f), whiteFadeUI_Material);
    pressSpaceUI = std::make_unique<VisualPlane>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.5f, 0.5f, 0.5f), pressSpaceUI_Material);
    
    
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

    test_cube = std::make_unique<VisualCube>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), glm::vec3(0.0f), test_material);

    // Define terrain parameters
    int grid_size = 100;
    float iso_surface_level = 0.0f;

    // FOR GETTING XZ RELATED Y WE CAN JUST HAVE SMALL CHUNKS , CHECK WHAT CHUNK WERE IN AND LOOP THROUGH VERTICES OF THAT CHUNK

    Shader::Material terrain_material = {
            .shader = terrain_lit_shader,
            .color = glm::vec3(1.0f),
            .lights = lights
    };

    // Create the terrain generator
    main_terrain = std::make_unique<GenerateTerrain>(grid_size, iso_surface_level, glm::vec3(0.0f,0.0f,0.0f), 0, terrain_material);

     //rock cube
    Shader::Material temp_rock_material = {
            .shader = lit_shader,
            .line_thickness = 3.0f,
            .color = glm::vec3(0.0f, 0.0f, 1.0f),
            .lights = lights,
    };

    rock_material = std::make_unique<Shader::Material>(temp_rock_material);

    rock_cube = std::make_unique<VisualCube>(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), temp_rock_material);


    //leaf
    Shader::Material Leaf_material =  {
            .shader = lit_shader,
            .line_thickness = 3.0f,
            //.color = glm::vec3(0.0f, 0.0f, 1.0f),
            .lights = lights,
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

    boat_material = std::make_unique<Shader::Material>();
    boat_material->shader = model_shader;
    boat_material->lights = lights;
    boat_material->line_thickness = 3.0f;
    boat_material->color = glm::vec3(1.0f, 1.0f, 1.0f);
    boat_material->texture_influence = 1.0f;
    boatTexture = Texture::Library::CreateTexture("assets/textures/boatTexture.png");
    //boat_material->texture = Texture::Library::CreateTexture("assets/textures/boatTexture.png");
    boat = std::make_unique<VisualModel>("assets/models/boat.obj", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), *boat_material);

    fishingRod_material = std::make_unique<Shader::Material>();
    fishingRod_material->shader = model_shader;
    fishingRod_material->lights = lights;
    fishingRod_material->line_thickness = 3.0f;
    fishingRod_material->color = glm::vec3(1.0f, 1.0f, 1.0f);
    fishingRod_material->texture_influence = 1.0f;
    objectTexture = Texture::Library::CreateTexture("assets/textures/objectTexture.png");
    //boat_material->texture = Texture::Library::CreateTexture("assets/textures/boatTexture.png");
    fishingRod = std::make_unique<VisualModel>("assets/models/fishingRod.obj", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), *fishingRod_material);


    fishingRodTurn_material = std::make_unique<Shader::Material>();
    fishingRodTurn_material->shader = model_shader;
    fishingRodTurn_material->lights = lights;
    fishingRodTurn_material->line_thickness = 3.0f;
    fishingRodTurn_material->color = glm::vec3(1.0f, 1.0f, 1.0f);
    fishingRodTurn_material->texture_influence = 1.0f;
    //objectTexture = Texture::Library::CreateTexture("assets/textures/objectTexture.png");
    //boat_material->texture = Texture::Library::CreateTexture("assets/textures/boatTexture.png");
    fishingRodTurn = std::make_unique<VisualModel>("assets/models/fishingRod_turn.obj", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), *fishingRodTurn_material);

    fishWire_material = std::make_unique<Shader::Material>();
    fishWire_material->shader = model_shader;
    fishWire_material->lights = lights;
    fishWire_material->line_thickness = 3.0f;
    fishWire_material->color = glm::vec3(1.0f, 1.0f, 1.0f);
    fishWire_material->texture_influence = 1.0f;
    //objectTexture = Texture::Library::CreateTexture("assets/textures/objectTexture.png");
    //boat_material->texture = Texture::Library::CreateTexture("assets/textures/boatTexture.png");
    fishWire = std::make_unique<VisualModel>("assets/models/fishWire.obj", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), *fishWire_material);


    fishStatic_material = std::make_unique<Shader::Material>();
    fishStatic_material->shader = model_shader;
    fishStatic_material->lights = lights;
    fishStatic_material->line_thickness = 3.0f;
    fishStatic_material->color = glm::vec3(1.0f, 1.0f, 1.0f);
    fishStatic_material->texture_influence = 1.0f;
    //objectTexture = Texture::Library::CreateTexture("assets/textures/objectTexture.png");
    //boat_material->texture = Texture::Library::CreateTexture("assets/textures/boatTexture.png");
    fishStatic = std::make_unique<VisualModel>("assets/models/fish_static.obj", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), *fishWire_material);

    seagull_material = std::make_unique<Shader::Material>();
    seagull_material->shader = model_shader;
    seagull_material->lights = lights;
    seagull_material->line_thickness = 3.0f;
    seagull_material->color = glm::vec3(1.0f, 1.0f, 1.0f);
    seagull_material->texture_influence = 1.0f;
    //objectTexture = Texture::Library::CreateTexture("assets/textures/objectTexture.png");
    //boat_material->texture = Texture::Library::CreateTexture("assets/textures/boatTexture.png");
    seagull = std::make_unique<VisualModel>("assets/models/seagull_flying.obj", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), *seagull_material);

    seagullStand_material = std::make_unique<Shader::Material>();
    seagullStand_material->shader = model_shader;
    seagullStand_material->lights = lights;
    seagullStand_material->line_thickness = 3.0f;
    seagullStand_material->color = glm::vec3(1.0f, 1.0f, 1.0f);
    seagullStand_material->texture_influence = 1.0f;
    //objectTexture = Texture::Library::CreateTexture("assets/textures/objectTexture.png");
    //boat_material->texture = Texture::Library::CreateTexture("assets/textures/boatTexture.png");
    seagullStand = std::make_unique<VisualModel>("assets/models/seagull_standing.obj", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), *seagull_material);


    skybox_material = std::make_unique<Shader::Material>();
    skybox_material->shader = skybox_shader;
    skybox_material->lights = lights;
    skybox_material->line_thickness = 3.0f;
    skybox_material->color = glm::vec3(1.0f, 1.0f, 1.0f);
    skybox_material->texture_influence = 1.0f;
    skybox = std::make_unique<VisualModel>("assets/models/skybox.obj", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), *skybox_material);


    //shark
    default_material =  std::make_unique<Shader::Material>();
    default_material->shader = lit_shader;
    default_material->lights = lights;
    default_material->line_thickness = 3.0f;

    fish = std::vector<Fish>();
    sharks = std::vector<Shark>();
/*    for (int i = 0; i < 10; ++i) {
        fish.emplace_back(50.0f);
    }*/

    generateAnimals();

    // SOUNDS

    main_theme_buffer = std::make_unique<sf::SoundBuffer>();
    underwater_sfx_buffer = std::make_unique<sf::SoundBuffer>();
    transition_buffer = std::make_unique<sf::SoundBuffer>();
    waves_buffer = std::make_unique<sf::SoundBuffer>();
    fishingRod_buffer = std::make_unique<sf::SoundBuffer>();

    if (!main_theme_buffer->loadFromFile("assets/sounds/main_theme.mp3")) {
        std::cerr << "Failed to load main theme sound!" << std::endl;
    }

    if (!underwater_sfx_buffer->loadFromFile("assets/sounds/underwater_sfx.mp3")) {
        std::cerr << "Failed to load underwater sfx sound!" << std::endl;
    }

    if (!transition_buffer->loadFromFile("assets/sounds/transition.mp3")) {
        std::cerr << "Failed to load transition sfx sound!" << std::endl;
    }

    if (!waves_buffer->loadFromFile("assets/sounds/waves.mp3")) {
        std::cerr << "Failed to load waves sfx sound!" << std::endl;
    }

    if (!fishingRod_buffer->loadFromFile("assets/sounds/fishingRod.mp3")) {
        std::cerr << "Failed to load fishingRod sfx sound!" << std::endl;
    }

    main_theme = std::make_unique<sf::Sound>();
    underwater_sfx = std::make_unique<sf::Sound>();
    transition_sfx = std::make_unique<sf::Sound>();
    waves_sfx = std::make_unique<sf::Sound>();
    fishingRod_sfx = std::make_unique<sf::Sound>();

    main_theme->setBuffer(*main_theme_buffer);
    underwater_sfx->setBuffer(*underwater_sfx_buffer);
    transition_sfx->setBuffer(*transition_buffer);
    waves_sfx->setBuffer(*waves_buffer);
    fishingRod_sfx->setBuffer(*fishingRod_buffer);

    main_theme->setVolume(100.0f);
    underwater_sfx->setVolume(100.0f);
    transition_sfx->setVolume(100.0f);
    waves_sfx->setVolume(100.0f);
    fishingRod_sfx->setVolume(100.0f);

    main_theme->setLoop(true);
    underwater_sfx->setLoop(true);
    transition_sfx->setLoop(false);
    waves_sfx->setLoop(true);
    fishingRod_sfx->setLoop(false);
}

void Renderer::CreateSpawnMap(){

    //findMatchingYValues(float x, float z)

    //it is a grid of 100x100
    //we go through a bunch of xz points and find appropraite y
    //check normal to make sure we can spawn
    //loop through smaller grids that way we can populate in famillies

    //we need to return an array of an array of information to spawn

    float grid_size = 100;
    int skip_size = 10;

    srand(time(NULL));

    // object generation variables
    int type = 0;
    float y;
    int spawnd = 0;

    int to_spawn; //75% chance?
    int to_spawn_2;

    glm::vec3 color_chosen;
    glm::vec3 color_chosen_2;
    glm::vec3 color_chosen_3;


    glm::vec3 petal_color_choice[4] = {glm::vec3(0.424, 0.949, 0.851), glm::vec3(1, 0.416, 0.753), glm::vec3(0.98, 0.839, 0.322), glm::vec3(0.631, 0.506, 0.929)};
    glm::vec3 stem_color_choice[3] = {glm::vec3(0.172, 0.84, 0.204), glm::vec3(0.448, 1.0, 0.478), glm::vec3(0.674, 1.0, 0.701) };

    glm::vec3 clam_color_choice[3] = {glm::vec3(106.0f/255.0f, 90.0f/255.0f, 205.0f/255.0f), glm::vec3(1.0f,0.51f,0.612f), glm::vec3(1.0f,0.627f,0.129f)}; //shell
    glm::vec3 clam_color_choice_2[3] = {glm::vec3(181.0f/255.0f, 165.0f/255.0f, 213.0f/255.0f), glm::vec3(1.0f,0.627f,0.702f), glm::vec3(1.0f,0.757f,0.435f)}; //pearl ... really means secondary

    glm::vec3 coral_color_choice_1[5] = {glm::vec3(1.0f, 0.7f, 0.2f),glm::vec3(0.0f, 1.0f, 0.816f),glm::vec3(0.878f, 0.322f, 0.424f), glm::vec3(0.565f, 0.871f, 0.282f),glm::vec3(1.0f, 0.475f, 0.396f)}; //darkest
    glm::vec3 coral_color_choice_2[5] = {glm::vec3(1.0f, 0.8f, 0.3f),glm::vec3(0.435, 1.0f, 0.898f),glm::vec3(1.0f, 0.231f, 0.373f),glm::vec3(0.663f, 1.0f, 0.357f),glm::vec3(1.0f, 0.596f, 0.537f)}; //dark
    glm::vec3 coral_color_choice_3[5] = {glm::vec3(1.0f, 1.0f, 0.4f),glm::vec3(0.663f, 0.988f, 0.929f),glm::vec3(1.0f, 0.376f, 0.49f),glm::vec3(0.733f, 1.0f, 0.49f),glm::vec3(1.0f, 0.769f, 0.733f)}; //light
    float scaleF;

    int max_flower_spawn;
    float height;

    spawn_list_Global.clear();
    light_placed = -1;

    for (int g_row = 0; g_row < grid_size; g_row += skip_size) {
        for (int g_col = 0; g_col < grid_size; g_col += skip_size) {
            to_spawn = rand() % 4; //75%
            std::cout<<"chance to spawn in chunk : "<<to_spawn<<std::endl;
            if (to_spawn > 0) {
                //random type
                int type = rand() % 9; //gives 1 number representing the object that will spawn
                std::cout<<"spawns a type : "<<type<<std::endl;

                //rest spawnd number
                spawnd = 0;

                //now give criteria based on type
                switch(type) {
                    case 0: // generic seaweed
                    //seaweed 1 , generic one
                    //make a familly spawn

                    //we have to spawn similar seaweed in a bunch to make it look real
                    //our grid is a 10x10
                        for (float x = g_row; x < g_row + skip_size; x+=0.75) {
                            for (float z = g_col; z < g_col + skip_size; z+=0.75) {
                                // check y value
                                std::vector<float> valid_y;
                                std::vector<YAndNormal> position_values = main_terrain->FindMatchingYValues(x,z);
                                for (auto & pos : position_values) {
                                    float y = pos.y;
                                    float norm_y = pos.normal;
                                    if (norm_y > 0.3f) {
                                        valid_y.push_back(y);
                                    }
                                }
                                if (!valid_y.empty()) {
                                    // get y value
                                    int index_y = rand() % (valid_y.size());
                                    y = valid_y.at(index_y);

                                    //to spawn or not to spawn
                                    to_spawn_2 = rand() % 10;
                                    if (to_spawn_2 == 1) {
                                        ObjectProperties ObjectProperties{};
                                        //what to spawn
                                        //information in this order :
                                        // type, x_offset, color, scaleFactor , x,y,z
                                        //type we have
                                        ObjectProperties.type = type;
                                        //x_offset
                                        ObjectProperties.x_offset = 0.1f; // 0.03 -> 0.06
                                        std::cout<<"x_offset "<<ObjectProperties.x_offset<<std::endl;
                                        //color, pick one fromn the array
                                        int color_nb = rand() % 3;
                                        ObjectProperties.color1 = stem_color_choice[color_nb];
                                        std::cout<<"x color "<<ObjectProperties.color1.x <<std::endl;
                                        //scaleFactor
                                        ObjectProperties.scaleF = 0.6f; //leave default for now
                                        //pos
                                        ObjectProperties.pos = glm::vec3(x,y,z);
                                        std::cout<<"x pos "<<ObjectProperties.pos.x <<std::endl;

                                        // put into big list
                                        spawn_list_Global.push_back(ObjectProperties);
                                    }
                                }
                            }
                        }
                        break;
                    case 1: // tall seaweed
                        for (int x = g_row; x < g_row + skip_size; ++x) {
                            for (int z = g_col; z < g_col + skip_size; ++z) {
                                to_spawn_2 = rand() % 2;
                                if (to_spawn_2 == 1 && spawnd < 6) {
                                    //pos
                                    float position_x  = x + static_cast<int>(rand()) / RAND_MAX * (g_row - x);
                                    std::vector<float> valid_y;
                                    std::vector<YAndNormal> position_values = main_terrain->FindMatchingYValues(position_x,z);
                                    for (auto & pos : position_values) {
                                        float y = pos.y;
                                        float norm_y = pos.normal;
                                        if (norm_y > 0.3) {
                                            valid_y.push_back(y);
                                        }
                                    }
                                    if (!valid_y.empty()) {
                                        // get y value
                                        int index_y = rand() % (valid_y.size());
                                        y = valid_y.at(index_y);

                                        ObjectProperties ObjectProperties{};
                                        //type we have
                                        ObjectProperties.type = type;
                                        //x_offset
                                        ObjectProperties.height  = rand() % 21 + 60 ; // 60 - 80;
                                        //color, pick one fromn the array
                                        int color_nb = rand() % 3;
                                        ObjectProperties.color1 = stem_color_choice[color_nb];

                                        color_nb = rand() % 4;
                                        ObjectProperties.color2 = petal_color_choice[color_nb];
                                        //scaleFactor
                                        ObjectProperties.scaleF = 0.9f; //leave default for now

                                        //pos
                                        ObjectProperties.pos = glm::vec3(position_x,y,z);

                                        // put into big list
                                        spawn_list_Global.push_back(ObjectProperties);
                                        spawnd += 1;
                                    }
                                }
                            }
                        }

                        break;
                    case 2: // clam
                        for (int x = g_row; x < g_row + skip_size; ++x) {
                            for (int z = g_col; z < g_col + skip_size; ++z) {

                                to_spawn_2 = rand() % 2;
                                if (to_spawn_2 == 1 && spawnd < 1) {
                                    //pos
                                    float position_x  = x + static_cast<int>(rand()) / RAND_MAX * (g_row - x);
                                    std::vector<float> valid_y;
                                    std::vector<YAndNormal> position_values = main_terrain->FindMatchingYValues(position_x,z);
                                    for (auto & pos : position_values) {
                                        float y = pos.y;
                                        float norm_y = pos.normal;
                                        if (norm_y > 0.3) {
                                            valid_y.push_back(y);
                                        }
                                    }
                                    if (!valid_y.empty()) {
                                        // get y value
                                        int index_y = rand() % (valid_y.size());
                                        y = valid_y.at(index_y);

                                        ObjectProperties ObjectProperties{};

                                        //type we have
                                        ObjectProperties.type = type;
                                        //scale
                                        ObjectProperties.scaleF = 0.9f; //0.3-0.7
                                        //color, pick one fromn the array
                                        int color_nb = rand() % 3;
                                        ObjectProperties.color1 = clam_color_choice[color_nb];
                                        ObjectProperties.color2 = clam_color_choice_2[color_nb];

                                        //pos
                                        ObjectProperties.pos = glm::vec3(position_x,y,z);

                                        // put into big list
                                        spawn_list_Global.push_back(ObjectProperties);
                                        spawnd += 1;
                                    }
                                }
                            }
                        }
                        break;
                    case 3: // coral 1
                        for (int x = g_row; x < g_row + skip_size; ++x) {
                            for (int z = g_col; z < g_col + skip_size; ++z) {

                                to_spawn_2 = rand() % 3;
                                if (to_spawn_2 == 1 && spawnd < 8) {

                                    //pos
                                    float position_x  = x + static_cast<int>(rand()) / RAND_MAX * (g_row - x);
                                    std::vector<float> valid_y;
                                    std::vector<YAndNormal> position_values = main_terrain->FindMatchingYValues(position_x,z);
                                    for (auto & pos : position_values) {
                                        float y = pos.y;
                                        float norm_y = pos.normal;
                                        if (norm_y > 0.3) {
                                            valid_y.push_back(y);
                                        }
                                    } 
                                    if (!valid_y.empty()) { 
                                        int index_y = rand() % (valid_y.size());
                                        y = valid_y.at(index_y) + 0.75f;
                                        

                                        ObjectProperties ObjectProperties{};

                                        //type we have
                                        ObjectProperties.type = type;
                                        //scale
                                        ObjectProperties.scaleF = 0.7; //0.3-0.7
                                        //color, pick one fromn the array
                                        int color_nb = rand() % 5;
                                        ObjectProperties.color1 = coral_color_choice_1[color_nb];
                                        ObjectProperties.color2 = coral_color_choice_2[color_nb];
                                        ObjectProperties.color3 = coral_color_choice_3[color_nb];

                                        //pos
                                        ObjectProperties.pos = glm::vec3(position_x,y,z);

                                        // put into big list
                                        spawn_list_Global.push_back(ObjectProperties);
                                        spawnd += 1;
                                    }
                                }
                            }
                        }
                        break;
                    case 4: // coral 2
                        for (int x = g_row; x < g_row + skip_size; ++x) {
                            for (int z = g_col; z < g_col + skip_size; ++z) {


                                to_spawn_2 = rand() % 3;
                                if (to_spawn_2 == 1 && spawnd < 10) {
                                    //pos
                                    float position_x  = x + static_cast<int>(rand()) / RAND_MAX * (g_row - x);
                                    std::vector<float> valid_y;
                                    std::vector<YAndNormal> position_values = main_terrain->FindMatchingYValues(position_x,z);
                                    for (auto & pos : position_values) {
                                        float y = pos.y;
                                        float norm_y = pos.normal;
                                        if (norm_y > 0.3) {
                                            valid_y.push_back(y);
                                        }
                                    }
                                    if (!valid_y.empty()) {
                                        // get y value
                                        int index_y = rand() % (valid_y.size());
                                        y = valid_y.at(index_y);

                                        ObjectProperties ObjectProperties{};

                                        //type we have
                                        ObjectProperties.type = type;
                                        //scale
                                        ObjectProperties.scaleF = 0.9f; //0.3-0.7
                                        //number of branches
                                        ObjectProperties.branches = rand() % 4 + 4; //4-7
                                        //color, pick one fromn the array
                                        int color_nb = rand() % 5;
                                        ObjectProperties.color1 = coral_color_choice_1[color_nb];
                                        ObjectProperties.color2 = coral_color_choice_2[color_nb];
                                        ObjectProperties.color3 = coral_color_choice_3[color_nb];

                                        //pos
                                        ObjectProperties.pos = glm::vec3(position_x,y,z);

                                        // put into big list
                                        spawn_list_Global.push_back(ObjectProperties);
                                        spawnd += 1;
                                    }
                                }
                            }
                        }
                        break;
                    case 5: //rock 1
                        for (int x = g_row; x < g_row + skip_size; x+=2) {
                            for (int z = g_col; z < g_col + skip_size; z+=2) {
                                to_spawn_2 = rand() % 5;
                                if (to_spawn_2 == 1 && spawnd < 2) {
                                    //pos
                                    float position_x  = x + static_cast<int>(rand()) / RAND_MAX * (g_row - x);
                                    std::vector<float> valid_y;
                                    std::vector<YAndNormal> position_values = main_terrain->FindMatchingYValues(position_x,z);
                                    for (auto & pos : position_values) {
                                        float y = pos.y;
                                        float norm_y = pos.normal;
                                        if (norm_y > 0.3) {
                                            valid_y.push_back(y);
                                        }
                                    }
                                    if (!valid_y.empty()) {
                                        // get y value
                                        int index_y = rand() % (valid_y.size());
                                        y = valid_y.at(index_y);

                                        ObjectProperties ObjectProperties{};

                                        //type we have
                                        ObjectProperties.type = type;
                                        //scale
                                        ObjectProperties.scaleF = 0.3f; //0.3-0.7

                                        //pos
                                        ObjectProperties.pos = glm::vec3(position_x,y,z);

                                        // put into big list
                                        spawn_list_Global.push_back(ObjectProperties);
                                        spawnd += 1;
                                    }
                                }

                            }
                        }
                        break;
                    case 6: //rock 2
                        for (int x = g_row; x < g_row + skip_size; x+=2) {
                            for (int z = g_col; z < g_col + skip_size; z+=2) {
                                to_spawn_2 = rand() % 7;
                                if (to_spawn_2 == 1 && spawnd < 3) {
                                    //pos
                                    float position_x  = x + static_cast<int>(rand()) / RAND_MAX * (g_row - x);
                                    std::vector<float> valid_y;
                                    std::vector<YAndNormal> position_values = main_terrain->FindMatchingYValues(position_x,z);
                                    for (auto & pos : position_values) {
                                        float y = pos.y;
                                        float norm_y = pos.normal;
                                        if (norm_y > 0.3) {
                                            valid_y.push_back(y);
                                        }
                                    }
                                    if (!valid_y.empty()) {
                                        // get y value
                                        int index_y = rand() % (valid_y.size());
                                        y = valid_y.at(index_y);

                                        ObjectProperties ObjectProperties{};

                                        //type we have
                                        ObjectProperties.type = type;
                                        //scale
                                        ObjectProperties.scaleF = 0.3f;

                                        //pos
                                        ObjectProperties.pos = glm::vec3(position_x,y,z);

                                        // put into big list
                                        spawn_list_Global.push_back(ObjectProperties);
                                        spawnd += 1;
                                    }
                                }
                            }
                        }
                        break;
                    case 7: //rock 3
                        for (int x = g_row; x < g_row + skip_size; x+=2) {
                            for (int z = g_col; z < g_col + skip_size; z+=2) {
                                to_spawn_2 = rand() % 8;
                                if (to_spawn_2 == 1 && spawnd < 4) {
                                    //pos
                                    float position_x  = x + static_cast<int>(rand()) / RAND_MAX * (g_row - x);
                                    std::vector<float> valid_y;
                                    std::vector<YAndNormal> position_values = main_terrain->FindMatchingYValues(position_x,z);
                                    for (auto & pos : position_values) {
                                        float y = pos.y;
                                        float norm_y = pos.normal;
                                        if (norm_y > 0.3) {
                                            valid_y.push_back(y);
                                        }
                                    }
                                    if (!valid_y.empty()) {
                                        // get y value
                                        int index_y = rand() % (valid_y.size());
                                        y = valid_y.at(index_y);

                                        ObjectProperties ObjectProperties{};

                                        //type we have
                                        ObjectProperties.type = type;
                                        //scale
                                        ObjectProperties.scaleF = 0.3f; //0.3-0.7
                                        //pos
                                        ObjectProperties.pos = glm::vec3(position_x,y,z);

                                        // put into big list
                                        spawn_list_Global.push_back(ObjectProperties);
                                        spawnd += 1;
                                    }
                                }
                            }
                        }
                        break;
                    case 8: //pebble
                        for (int x = g_row; x < g_row + skip_size; x+=2) {
                            for (int z = g_col; z < g_col + skip_size; z+=2) {
                                to_spawn_2 = rand() % 10;
                                if (to_spawn_2 == 1 && spawnd < 8) {
                                    //pos
                                    float position_x  = x + static_cast<int>(rand()) / RAND_MAX * (g_row - x);
                                    std::vector<float> valid_y;
                                    std::vector<YAndNormal> position_values = main_terrain->FindMatchingYValues(position_x,z);
                                    for (auto & pos : position_values) {
                                        float y = pos.y;
                                        float norm_y = pos.normal;
                                        if (norm_y > 0.3) {
                                            valid_y.push_back(y);
                                        }
                                    }
                                    if (!valid_y.empty()) {
                                        // get y value
                                        int index_y = rand() % (valid_y.size());
                                        y = valid_y.at(index_y);

                                        ObjectProperties ObjectProperties{};

                                        //type we have
                                        ObjectProperties.type = type;
                                        //scale
                                        ObjectProperties.scaleF = 0.3f; //0.3-0.7

                                        //pos
                                        ObjectProperties.pos = glm::vec3(position_x,y,z);

                                        // put into big list
                                        spawn_list_Global.push_back(ObjectProperties);
                                        spawnd += 1;
                                    }
                                }
                            }
                        }
                        break;
                }
            }
        }
    }
}

void Renderer::SpawnAllObjects(const Shader::Material* _material){
    // confirmed that we are spawning in this chunk
    //now the way and where we spawn depends on the object we roll
    if (light_placed == -1) light_placed = lights->size() - 1;

    for (auto & object : spawn_list_Global) {
        int type = object.type;
        // std::cout<<"HELOOOOO"<<type<<std::endl;
        switch(type) {
            case 0: // basic seaweed
                if (light_placed > 0 && rand() % 100 > 97) {
                    lights->at(light_placed).SetPosition(object.pos + glm::vec3(-1.0f, 5.0f, 2.0f));
                    lights->at(light_placed).SetTarget(object.pos);
                    light_placed--;
                }

                DrawOneWeed(object.pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(object.scaleF), main_camera->GetViewProjection(), main_camera->GetPosition(), _material, moving_angle , object.x_offset , object.color1 , glm::vec3(0.3f, 0.3f, 0.3f));
                // std::cout<<"draw seawwed"<<std::endl;
                // std::cout<<object.pos.x << " y " << object.pos.y << " z" << object.pos.z<<std::endl;
                break;
            case 1: // tall seaweed
                if (light_placed > 0 && rand() % 100 > 97) {
                    lights->at(light_placed).SetPosition(object.pos + glm::vec3(-1.0f, 5.0f, 2.0f));
                    lights->at(light_placed).SetTarget(object.pos);
                    light_placed--;
                }

                DrawOneWeed2(object.pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(object.scaleF), main_camera->GetViewProjection(), main_camera->GetPosition(), _material, moving_angle, object.height, object.color1, object.color2);
                // std::cout<<"draw plant"<<std::endl;
                // std::cout<<object.pos.x << " y " << object.pos.y << " z" << object.pos.z<<std::endl;
                break;
            case 2: // clam
                if (light_placed > 0 && rand() % 100 > 97) {
                    lights->at(light_placed).SetPosition(object.pos + glm::vec3(2.0f, 5.0f, 1.0f));
                    lights->at(light_placed).SetTarget(object.pos);
                    light_placed--;
                }

                DrawOneClam(object.pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(object.scaleF), main_camera->GetViewProjection(), main_camera->GetPosition(), _material, moving_angle, object.color1, object.color2);
                // std::cout<<"draw clam"<<std::endl;
                // std::cout<<object.pos.x << " y " << object.pos.y << " z" << object.pos.z<<std::endl;
                break;
            case 3: // coral 1
                if (light_placed > 0 && rand() % 100 > 97) {
                    lights->at(light_placed).SetPosition(object.pos + glm::vec3(-1.0f, 5.0f, 2.0f));
                    lights->at(light_placed).SetTarget(object.pos);
                    light_placed--;
                }

                DrawOneCoral(object.pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(object.scaleF), main_camera->GetViewProjection(), main_camera->GetPosition(), _material, moving_angle, object.color1, object.color2, object.color3);
                // std::cout<<"draw coral"<<std::endl;
                // std::cout<<object.pos.x << " y " << object.pos.y << " z" << object.pos.z<<std::endl;
                break;
            case 4: // coral 2
                if (light_placed > 0 && rand() % 100 > 97) {
                    lights->at(light_placed).SetPosition(object.pos + glm::vec3(-2.0f, 5.0f, -1.0f));
                    lights->at(light_placed).SetTarget(object.pos);
                    light_placed--;
                }
            // std::cout<<"HELOOOOO"<<type<<std::endl;
                DrawOneCoral2(object.pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(object.scaleF), main_camera->GetViewProjection(), main_camera->GetPosition(), _material, moving_angle, object.color1, object.color2, object.color3, object.branches);
                // std::cout<<"draw coral 2"<<std::endl;
                // std::cout<<object.pos.x << " y " << object.pos.y << " z" << object.pos.z<<std::endl;
                break;
            case 5: //rock 1
                DrawRock(object.pos, glm::vec3(0.0f), glm::vec3(object.scaleF), main_camera->GetViewProjection(), main_camera->GetPosition(), rock_material.get(), moving_angle/ 20.0f);
                break;
            case 6: //rock 2
                DrawRock2(object.pos, glm::vec3(0.0f), glm::vec3(object.scaleF), main_camera->GetViewProjection(), main_camera->GetPosition(), rock_material.get(), moving_angle/ 20.0f);
                break;
            case 7: //rock 3
                DrawRock3(object.pos, glm::vec3(0.0f), glm::vec3(object.scaleF), main_camera->GetViewProjection(), main_camera->GetPosition(), rock_material.get(), moving_angle/ 20.0f);
                break;
            case 8: //pebble
                DrawPebble(object.pos, glm::vec3(0.0f), glm::vec3(object.scaleF), main_camera->GetViewProjection(), main_camera->GetPosition(), rock_material.get(), moving_angle/ 20.0f);
                break;
        }
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

    VisualCube::SetupGlBuffersInstanced();
}

void Renderer::Render(GLFWwindow* _window, const double _deltaTime) {
    const auto current_time = (float)glfwGetTime();

    // processes input
    InputCallback(_window, _deltaTime);
    //std::cout<<"delta: "<<_deltaTime<< std::endl;
    //std::cout<<"framerate: " << 1 / _deltaTime   << std::endl;

    if (!is_underwater) {
        DrawIntroScene(_window, current_time, _deltaTime);
        return;
    }

    //animation
    moving_angle = glfwGetTime() * 20.0f;

    // SHADOW MAP PASS
    
    // binds the shadow map framebuffer and the depth texture to draw on it
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
    glViewport(0, 0, Light::LIGHTMAP_SIZE, Light::LIGHTMAP_SIZE);

    glm::mat4 first_world_transform_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f));
    first_world_transform_matrix = glm::scale(first_world_transform_matrix, glm::vec3(2.0f));

    glm::mat4 second_world_transform_matrix = glm::translate(glm::mat4(1.0f), main_camera->GetPosition());
    second_world_transform_matrix = glm::scale(second_world_transform_matrix, glm::vec3(500.0f));

    glm::mat4 third_world_transform_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 0.0f));
    third_world_transform_matrix = glm::scale(third_world_transform_matrix, glm::vec3(1.5f));

    for (int i = 0; i < 1; ++i) {
        const auto& light = lights->at(i);

        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_map_texture, 0, i);

        // clears the depth canvas to black
        glClear(GL_DEPTH_BUFFER_BIT);

        //todo: Draw shadow-caster elements HERE (i.e. the cubes below)

        SpawnAllObjects(shadow_mapper_material.get());

        //test_cube->DrawFromMatrix(light.GetViewProjection(), light.GetPosition(), first_world_transform_matrix, GL_TRIANGLES, shadow_mapper_material.get());

        test_cube->DrawFromMatrix(light.GetViewProjection(), light.GetPosition(), second_world_transform_matrix, current_time, GL_TRIANGLES, shadow_mapper_material.get());

        main_terrain->DrawChunk(light.GetViewProjection(), light.GetPosition(), glm::mat4(1.0f), current_time, GL_TRIANGLES, shadow_mapper_material.get());

        for (auto& one_fish : fish) {
            one_fish.DrawFromMatrix(light.GetViewProjection(), light.GetPosition(), glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)), current_time, GL_TRIANGLES, shadow_mapper_material.get());
        }
        for (auto& one_sharks : sharks) {
            one_sharks.DrawFromMatrix(light.GetViewProjection(), light.GetPosition(), glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)), current_time, GL_TRIANGLES, shadow_mapper_material.get());
        }

        VisualCube::DrawInstanced(light.GetViewProjection(), light.GetPosition(), current_time, GL_TRIANGLES, shadow_mapper_material.get());
    }

    // COLOR PASS

    // binds the screen to draw on it
    main_screen->Bind();

    // activates the shadow map depth texture & binds it to the second texture unit, so that it can be used for shadow mapping
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadow_map_texture);

    //todo: Draw colored elements HERE (i.e. the cubes, light cubes, grid, etc. below)
    {
        // draws the main grid
        // main_grid->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());

        // draws the coordinate axis
        main_x_line->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());
        main_y_line->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());
        main_z_line->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());

        test_cube->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), second_world_transform_matrix, current_time);

        main_terrain->DrawChunk(main_camera->GetViewProjection(), main_camera->GetPosition(), glm::mat4(1.0f), current_time);

        SpawnAllObjects(default_material.get());

        for (auto& one_fish : fish) {
            one_fish.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)), current_time, GL_TRIANGLES, default_material.get());
        }
        for (auto& one_sharks : sharks) {
            one_sharks.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)), current_time, GL_TRIANGLES, default_material.get());
        }

        VisualCube::DrawInstanced(main_camera->GetViewProjection(), main_camera->GetPosition(), current_time, GL_TRIANGLES, default_material.get());
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

    glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbinds the shadow map framebuffer
    

    if (underwaterStartTime == -1)
        underwaterStartTime = current_time;

    if (Input::IsKeyReleased(_window, GLFW_KEY_SPACE) && cutscenePressedSpace == false) {
        cutscenePressedSpace = true;
        transition_sfx->play();
        underwaterStartTime = current_time;
        return;
    }

    if (cutscenePressedSpace == true)
    {
        float soundVolume = glm::clamp(1 - (current_time - underwaterStartTime), 0.0f, 1.0f);
        main_theme->setVolume(soundVolume);
        underwater_sfx->setVolume(soundVolume);
        if (current_time - underwaterStartTime > 3.0f)
            SwitchScenes();
    }

    float startWhiteFadeValue = glm::clamp(1 - (current_time - underwaterStartTime), 0.0f, 1.0f);
    float endWhiteFadeValue = glm::clamp((current_time - underwaterStartTime), 0.0f, 1.0f);
    float finalWhiteFadeValue = cutscenePressedSpace ? endWhiteFadeValue : startWhiteFadeValue;
    whiteFadeUI->material.alpha = finalWhiteFadeValue;
    if (finalWhiteFadeValue > 0.0f) {
        pressSpaceUI->material.shader->SetFloat("wobbleFactor", 0.0f);
        whiteFadeUI->DrawFromMatrix(glm::mat4(1.0f), glm::vec3(1.0f), glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)), _deltaTime);
    }
}

float Renderer::getRandomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

void Renderer::generateAnimals() {
    fish.clear();
    sharks.clear();
    for (int i = 0; i < 40; ++i) {
        float coin = getRandomFloat(-30.0f, 30.0f);
        if (coin < 20.0f) {
            fish.emplace_back(getRandomFloat(4.0f,6.0f));
        }
        else {
            sharks.emplace_back(getRandomFloat(8.0f, 9.0f));
        }

    }

}

void Renderer::ResizeCallback(GLFWwindow* _window, int _displayWidth, int _displayHeight) {
    // updates the viewport size
    viewport_width = _displayWidth;
    viewport_height = _displayHeight;

    main_camera->SetViewportSize((float)_displayWidth, (float)_displayHeight);
    main_screen->ResizeCallback(_displayWidth, _displayHeight);
}

void Renderer::SwitchScenes() {
    is_underwater = !is_underwater;

    if (is_underwater) {
        waves_sfx->stop();
        main_terrain->GenerateChunkTerrain(true);
        main_terrain->SetupBuffers();

        CreateSpawnMap();
        main_theme->setVolume(100.0f);
        underwater_sfx->setVolume(100.0f);
        main_theme->play();
        underwater_sfx->play();

        main_camera->SetPosition(glm::vec3(50.0f, 40.0f, 50.0f));
        main_camera->SetTarget(glm::vec3(30.0f));

        underwaterStartTime = -1;
        cutscenePressedSpace = false;
    } else {
        main_theme->stop();
        underwater_sfx->stop();
        
        cutsceneStartTime = -2;
        cutscenePressedSpace = false;
        fishingRodThrown = false;
    }
}

//flag
void Renderer::DrawIntroScene(GLFWwindow* _window, const double _time, const double _deltaTime) {
    float panDuration = 13.5f;

    if (cutsceneStartTime == -1) {
        //std::cout << "-1\n";
        cutsceneStartTime = _time;
        waves_sfx->setVolume(100.0f);
        waves_sfx->play();
        fishWire->position = glm::vec3(0.0f, -1000.0f, 0.0f);
        fishStatic->position = glm::vec3(0.0f, -1000.0f, 0.0f);
    }

    if(cutsceneStartTime == -2) 
    {
        //std::cout << "-2\n";
        cutsceneStartTime = _time - panDuration;
        waves_sfx->setVolume(100.0f);
        waves_sfx->play();
        fishWire->position = glm::vec3(0.0f, -1000.0f, 0.0f);
        fishStatic->position = glm::vec3(0.0f, -1000.0f, 0.0f);
    }

    float realTime = _time - cutsceneStartTime;

    glm::vec3 camPos = glm::vec3(15, 15, 5);
    glm::vec3 camTarget = glm::vec3(0, 0, 0);

    glm::vec3 startPanPos = glm::vec3(15, 15, 5);
    glm::vec3 endPanPos = glm::vec3(6, 6, 1);

    
    if (realTime < 1) {
        titleScreenUI->material.alpha = 0.0f;
        blackFadeUI->material.alpha = 1.0f;
    }

    
    float blackValue = glm::clamp(1 - ((realTime - 1) / 1.0f), 0.0f, 1.0f);
    float uiValue = 0.0f;
    float pressSpaceValue = 0.0f;
    if (realTime >= 1 && realTime <= panDuration) {
        float value = (realTime - 1) / panDuration;
        camPos = glm::mix(startPanPos, endPanPos, value);

        uiValue = glm::clamp((realTime-4) / 3, 0.0f, 1.0f);
        if(realTime>=10)
            uiValue = glm::clamp( 1-((realTime - 10) / 1.5f), 0.0f, 1.0f);
        titleScreenUI->material.alpha = uiValue;
        blackFadeUI->material.alpha = blackValue;

        seagull->position = glm::vec3(10, 10+sin(realTime*5)*0.2f, -10) + glm::vec3(0, 0, 1) * (realTime - 1) * 5.0f;
        seagull->rotation = glm::vec3(0, 200, 0);

        seagullStand->position = glm::vec3(1.1f, 0.72f, 2.7f);
        seagullStand->rotation = glm::vec3(0.0f, -110.0f, 0.0f);
        seagullStand->scale = glm::vec3(0.5f);
    }

    float fishingRodThrowDuration = 16;
    if (realTime >= panDuration && realTime < fishingRodThrowDuration)
    {
        if (fishingRodThrown == false)
        {
            fishingRod_sfx->play();
            fishingRodThrown = true;
        }
        float rotValue = sin( (realTime - panDuration) * 5.0f);
        if ((realTime - panDuration) * 5.0f > 1.8)
            rotValue = sin(1.8);

        camPos = glm::vec3(-0.5, 1.5, 2.65);
        camTarget = glm::vec3(-0.4, 1.5, 2.7);

        fishingRod->position = glm::vec3(0, 1, 2.5);
        fishingRod->rotation = glm::vec3(-45.0f * rotValue, 0.0f, 0.0f);
    }

    float fishingRodTurnDuration = 19;
    if (realTime >= fishingRodThrowDuration && realTime < fishingRodTurnDuration)
    {
        float t = realTime - fishingRodThrowDuration;
         
        fishWire->position = glm::vec3(0, 0.96 + t*0.05f,  2.9);

        camPos = glm::vec3(0.2, 1.1, 2.8);
        camTarget = glm::vec3(0, 1.15, 2.9);

    }

    glm::vec3 fishInPos = glm::vec3(0.1, 0.55f, 2.55);
    float fishFallDuration = 19;
    /*if (realTime >= fishingRodTurnDuration && realTime < fishFallDuration)
    {
        float value = realTime - fishingRodTurnDuration;
        camPos = glm::vec3(0, 0.7, 2.5);
        camTarget = glm::vec3(0.1, 0.55, 2.55);

        float ampl = 1.0f;
        float hitTime = 1.5707f / 5.0f;
        if (value > hitTime + hitTime * 5)
            ampl = 0.0f;
        else if (value > hitTime + hitTime*2)
            ampl = 0.01f;
        else if (value > hitTime)
            ampl = 0.05f;

        if (value > hitTime + hitTime * 5)

        fishStatic->position = glm::vec3(0.1, 0.55 + glm::abs(cos(value * 5.0f)) * ampl, 2.55);
        fishStatic->rotation = glm::vec3(0.0f, 30.0f, 0.0f);
    }*/

    
    if (realTime >= fishFallDuration && !cutscenePressedSpace)
    {
        if (Input::IsKeyReleased(_window, GLFW_KEY_SPACE)) {
            cutscenePressedSpace = true;
            cutsceneStartTime = _time - fishFallDuration;
            transition_sfx->play();
            return;
        }

        pressSpaceValue = glm::clamp((realTime - fishFallDuration) / 0.5f, 0.0f, 1.0f);
        pressSpaceUI->material.alpha = pressSpaceValue;

        camPos = glm::vec3(0, 0.7, 2.5);
        camTarget = glm::vec3(0.1, 0.55, 2.55);

        fishStatic->position = glm::vec3(0.1, 0.55, 2.55);
        fishStatic->rotation = glm::vec3(0.0f, 30.0f, 0.0f);
    }

    float whiteValue = 0.0f;
    float fishJumpDuration = 22;
    if (realTime >= fishFallDuration && cutscenePressedSpace)
    {
        float value = realTime - fishFallDuration;
        whiteValue = glm::clamp(((realTime - fishFallDuration - 0.2f) / 1.0f), 0.0f, 1.0f);
        whiteFadeUI->material.alpha = whiteValue;
        fishStatic->position = glm::vec3(0.1, 0.55 + value*0.6f, 2.55 + value*0.3f);
        fishStatic->rotation = glm::vec3(0.0f, 30.0f, value * 45.0f);

        float soundValue = glm::clamp( 1 - ((realTime - fishFallDuration) / 0.5f), 0.0f, 1.0f);
        waves_sfx->setVolume(soundValue*100);

        camPos = glm::vec3(0, 0.7, 2.5);
        camTarget = glm::vec3(0.1, 0.55, 2.55);
    }

    if (realTime >= fishJumpDuration && cutscenePressedSpace)
    {
        //std::cout << "time: " << realTime << "\n";
        SwitchScenes();
        return;
    }


    if (animateCamera) {
        main_camera->SetPosition(camPos);
        main_camera->SetTarget(camTarget);
    }




    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
    glViewport(0, 0, Light::LIGHTMAP_SIZE, Light::LIGHTMAP_SIZE);

    for (int i = 0; i < lights->size(); ++i) {
        const auto& light = lights->at(i);

        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_map_texture, 0, i);

        // clears the depth canvas to black
        glClear(GL_DEPTH_BUFFER_BIT);

        boat->Draw(main_camera->GetViewProjection(), main_camera->GetPosition(), (float)_time, GL_TRIANGLES, boat_material.get());
        main_ocean->Draw(main_camera->GetViewProjection(), main_camera->GetPosition(), (float)_time, GL_TRIANGLES, ocean_material.get());

    }

    // COLOR PASS

    // binds the screen to draw on it
    main_screen->Bind();

    // activates the shadow map depth texture & binds it to the second texture unit, so that it can be used for shadow mapping
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadow_map_texture);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // resets the viewport to the window size
    glViewport(0, 0, viewport_width, viewport_height);

    // clears the color & depth canvas to black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    main_skybox->UseCubemap(GL_TEXTURE1);
    skybox_material->shader->SetTexture("skybox", 1);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    {
        glm::vec3 camPos = main_camera->GetPosition();
        glm::vec3 camFrwd = main_camera->GetCamForward();
        //std::cout << "x: " << camFrwd.x << "y: " << camFrwd.y << "z: " << camFrwd.z << "\n";
        glm::vec3 camTarget = -camFrwd;
        glm::vec3 camOldTarget = camPos + camFrwd;
        //main_camera->SetPosition(glm::vec3(0.0f));
        //main_camera->SetTarget(camTarget);
        glm::mat4 skyView = main_camera->GetProjection() * glm::mat4(glm::mat3(main_camera->GetView()));
        skybox->Draw(skyView, main_camera->GetPosition(), (float)_time, GL_TRIANGLES, skybox_material.get());
        //main_camera->SetPosition(camPos);
        //main_camera->SetTarget(camOldTarget);
    }
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    main_skybox->UseCubemap(GL_TEXTURE1);
    ocean_flow_map->UseSingle(GL_TEXTURE2);

    ocean_material->shader->SetTexture("skybox", 1);
    ocean_material->shader->SetTexture("oceanFlowMap", 2);

    main_ocean->position = glm::vec3(-15.0f, 0.0f, -15.0f);
    main_ocean->Draw(main_camera->GetViewProjection(), main_camera->GetPosition(), (float)_time, GL_TRIANGLES, ocean_material.get());
    
    boatTexture->UseSingle(GL_TEXTURE3);
    boat_material->shader->SetTexture("u_texture", 3);
    boat_material->shader->SetFloat("u_texture_influence", 1.0f);
    boat->Draw(main_camera->GetViewProjection(), main_camera->GetPosition(), (float)_time, GL_TRIANGLES, boat_material.get());

    objectTexture->UseSingle(GL_TEXTURE3);
    fishingRod_material->shader->SetTexture("u_texture", 3);
    fishingRod_material->shader->SetFloat("u_texture_influence", 1.0f);
    fishingRod->Draw(main_camera->GetViewProjection(), main_camera->GetPosition(), (float)_time, GL_TRIANGLES, fishingRod_material.get());

    objectTexture->UseSingle(GL_TEXTURE3);
    fishingRodTurn_material->shader->SetTexture("u_texture", 3);
    fishingRodTurn_material->shader->SetFloat("u_texture_influence", 1.0f);
    fishingRodTurn->Draw(main_camera->GetViewProjection(), main_camera->GetPosition(), (float)_time, GL_TRIANGLES, fishingRodTurn_material.get());

    objectTexture->UseSingle(GL_TEXTURE3);
    fishWire_material->shader->SetTexture("u_texture", 3);
    fishWire_material->shader->SetFloat("u_texture_influence", 1.0f);
    fishWire->Draw(main_camera->GetViewProjection(), main_camera->GetPosition(), (float)_time, GL_TRIANGLES, fishWire_material.get());

    objectTexture->UseSingle(GL_TEXTURE3);
    fishStatic_material->shader->SetTexture("u_texture", 3);
    fishStatic_material->shader->SetFloat("u_texture_influence", 1.0f);
    fishStatic->Draw(main_camera->GetViewProjection(), main_camera->GetPosition(), (float)_time, GL_TRIANGLES, fishStatic_material.get());

    objectTexture->UseSingle(GL_TEXTURE3);
    seagull_material->shader->SetTexture("u_texture", 3);
    seagull_material->shader->SetFloat("u_texture_influence", 1.0f);
    seagull->Draw(main_camera->GetViewProjection(), main_camera->GetPosition(), (float)_time, GL_TRIANGLES, fishStatic_material.get());

    objectTexture->UseSingle(GL_TEXTURE3);
    seagullStand_material->shader->SetTexture("u_texture", 3);
    seagullStand_material->shader->SetFloat("u_texture_influence", 1.0f);
    seagullStand->Draw(main_camera->GetViewProjection(), main_camera->GetPosition(), (float)_time, GL_TRIANGLES, fishStatic_material.get());

    //UI
    if (blackValue > 0.0f) {
        pressSpaceUI->material.shader->SetFloat("wobbleFactor", 0.0f);
        blackFadeUI->DrawFromMatrix(glm::mat4(1.0f), glm::vec3(1.0f), glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)), _time);
    }
    if (whiteValue > 0.0f) {
        pressSpaceUI->material.shader->SetFloat("wobbleFactor", 0.0f);
        whiteFadeUI->DrawFromMatrix(glm::mat4(1.0f), glm::vec3(1.0f), glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)), _time);
    }
    if (uiValue > 0.0f) {
        titleScreenUI->material.shader->SetFloat("wobbleFactor", 1.0f);
        titleScreenUI->DrawFromMatrix(glm::mat4(1.0f), glm::vec3(1.0f), glm::scale(glm::mat4(1.0f), glm::vec3(0.7f)), _time);
    }
    if (pressSpaceValue > 0.0f)
    {
        pressSpaceUI->material.shader->SetFloat("wobbleFactor", 0.0f);
        pressSpaceUI->DrawFromMatrix(glm::mat4(1.0f), glm::vec3(1.0f), glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)), _time);
    }
    Texture::Clear();
}

void Renderer::DrawOneWeed(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection, const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time
    , float _x_offset, glm::vec3 _weedcolor , glm::vec3 _scale_factor ) {
    glm::mat4 world_transform_matrix = glm::mat4(1.0f);

    world_transform_matrix = glm::translate(world_transform_matrix, _position);
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, _rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, _scale);

    //glm::vec3(0.3f, 0.3f, 0.3f);
    auto scale_factor =  _scale_factor ;     // scale for one cube


    glm::mat4 secondary_transform_matrix = world_transform_matrix;

    //animation values
    //float moving_angle = glm::cos(glfwGetTime() * 2.0f);

    //base stem
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    //world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, _weedcolor);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);


    // float x_offset = 0.05f;
    // float rot_bounce = 90.0f;


    float x_offset = _x_offset;
    float y_offset = scale_factor.y;

    float bounce = 1.0f;
    float bounce_sin = 1.0f;

    float sin_offset = cos(time/20)/20;

    //glm::vec3 weedcolor = glm::vec3(0.197f, 0.321f, 0.118f);
    stem_cube->material.color = _weedcolor; // colour


    for(int x = 0; x < 10; x++){

        if(bounce_sin / 3.0f == 1.0f){
            bounce_sin = 1.0f;
            x_offset = -x_offset;
        }

        //draw stem block
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(x_offset+sin_offset, y_offset, 0.0f));
        world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
        VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, _weedcolor);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);


        //bounce
        bounce += 1.0f;
        bounce_sin += 1.0f;



    }


}


void Renderer::DrawOneWeed2(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection, const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time, float height, glm::vec3 _weedcolor, glm::vec3 _leafcolor)
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
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, _weedcolor);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    float x_offset = 0.025f;
    float y_offset = 0.1f;
    float rot_bounce = 90.0f;
    float bounce = 1.0f;
    float bounce_sin = 1.0f;
    int counter = 0;

    float sin_offset = cos(time/20)/20;

    //weed color
    glm::vec3 weedcolor = _weedcolor;
    stem_cube->material.color = weedcolor; // colour

    //leaf color
    //glm::vec3 leafcolor = glm::vec3(1.0f, 0.5f, 0.2f);
    leaf_cube->material.color = _leafcolor; // colour



    for(int x = 0; x < height; x++){

        if(bounce_sin / 2.0f == 1.0f){
            bounce_sin = 1.0f;
            x_offset = -x_offset;
        }

        //draw stem block
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(x_offset, y_offset, 0.0f));
        //world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, bounce * rot_bounce, 0.0f));
        world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
        VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, _weedcolor);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
        //world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, -bounce * rot_bounce, 0.0f));

        //draw leaf block

        if(counter == 5){
            world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, bounce * rot_bounce + sin_offset*25, 0.0f));
            world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 45.0f));

            leaf_cube->material.shader = _materialOverride->shader;
            DrawOneLeaf(secondary_transform_matrix, _viewProjection, _eyePosition, &leaf_cube->material);

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


void Renderer::DrawOneClam(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection, const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time, glm::vec3 _color1, glm::vec3 _color2 )
{
    glm::mat4 world_transform_matrix = glm::mat4(1.0f);

    world_transform_matrix = glm::translate(world_transform_matrix, _position);
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, _rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, _scale);

    auto scale_factor = glm::vec3(0.1f, 0.1f, 0.1f);        // scale for one cube
    auto scale_factor_lip = glm::vec3(0.1f, 0.1f, 0.1f);        // scale for one cube

    float sin_offset = abs(cos(time/60) * 70);

    glm::mat4 secondary_transform_matrix = world_transform_matrix;

    //new clam model
    //make varying heighted strips
    //and add a small lip
    clam_cube->material.color = _color1; // colour
    scale_factor = glm::vec3(0.1f, 0.1f, 1.0f);
    //make lip for that strand
    lip_cube->material.color = glm::vec3(255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f); // colour
    scale_factor_lip = glm::vec3(0.1f, 0.1f, 0.1f);


    pearl_cube->material.color = _color2; // colours

    //first strand is highest

    //shell
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.15f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor - glm::vec3(0.0f, 0.0f, 0.05f) );
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, clam_cube->material.color);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / (scale_factor- glm::vec3(0.0f, 0.0f, 0.05f)));
    //lip
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.05f, 0.525f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, lip_cube->material.color);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);

    //start patern

    //shell
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.1f, -0.1f, -0.50f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, clam_cube->material.color);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
    //lip
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.05f, 0.55f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, lip_cube->material.color);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);

    //autofill remainder
    for(int i = 1; i<7; i++){

        if( i % 2 == 0){ //if even

            //top strand
            world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.1f, 0.0f, -0.55f));
            world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
            VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, clam_cube->material.color);
            world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
            //lip
            world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.05f, 0.55f));
            world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
            VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, lip_cube->material.color);
            world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);

        }else{
            //bottom strand
            world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.1f, -0.1f, -0.55f));
            world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
            VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, clam_cube->material.color);
            world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
            //lip
            world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.05f, 0.55f));
            world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
            VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, lip_cube->material.color);
            world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);

        }

    }


    //do last strand 1 step higher

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.1f, 0.0f, -0.60f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor - glm::vec3(0.0f, 0.0f, 0.05f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, clam_cube->material.color);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / (scale_factor - glm::vec3(0.0f, 0.0f, 0.05f)));
    //lip
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.05f, 0.525f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, lip_cube->material.color);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);


    //draw pearl in middle

    //use lip since it is also white

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.4f, 0.0f, -0.45f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.2f, 0.2f, 0.2f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, lip_cube->material.color);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / glm::vec3(0.2f, 0.2f, 0.2f));

    //edges no lip
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.5f, 0.05f, -0.05f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor - glm::vec3(0.0f, 0.0f, 0.05f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, clam_cube->material.color);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / (scale_factor - glm::vec3(0.0f, 0.0f, 0.05f)));

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-1.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor - glm::vec3(0.0f, 0.0f, 0.05f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, clam_cube->material.color);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / (scale_factor - glm::vec3(0.0f, 0.0f, 0.05f)));

    //start top pattern
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.1f, 0.0f, -0.525f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip );
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, clam_cube->material.color);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip );

    for(int i = 0; i<8; i++){
        //make the back face top
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.1f, 0.0f, 0.0f));
        world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip );
        VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, clam_cube->material.color);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip );

    }

    //1 level lower
    //start pattern
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.1f, -0.1f, -0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip );
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, clam_cube->material.color);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip );

    for(int i = 0; i<6; i++){
        //make the back face top
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.1f, 0.0f, 0.0f));
        world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip );
        VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, clam_cube->material.color);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip );

    }


    //now repeat to get the top half

    //go up 2 move to middle make a line
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3( 0.3f, 0.2f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(sin_offset, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3( 0.9f, 0.1f, 0.1f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, clam_cube->material.color);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / glm::vec3( 0.9f, 0.1f, 0.1f) );
    //second back plate
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3( 0.0f, 0.05f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3( 0.7f, 0.05f, 0.1f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, clam_cube->material.color);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / glm::vec3( 0.7f, 0.05f, 0.1f) );

    //set up transition for new top strands
    //go back down 1 and start at the edge
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3( 0.4f, -0.05f, 0.50f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor - glm::vec3(0.0f, 0.0f, 0.05f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, clam_cube->material.color);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f /( scale_factor- glm::vec3(0.0f, 0.0f, 0.05f)));
    //make lip
    //lip_cube->material.color = glm::vec3(55.0f/255.0f, 25.0f/255.0f, 25.0f/255.0f); // colour
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3( 0.0f, -0.05f, 0.525f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, lip_cube->material.color);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);

    //now go 1 up
    //top strand
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.1f, 0.1f, -0.50f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, clam_cube->material.color);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
    //lip
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -0.05f, 0.55f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, lip_cube->material.color);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);

    //up and pearl color
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.1f, 0.1f, -0.55f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, pearl_cube->material.color);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
    //lip
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -0.05f, 0.55f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, lip_cube->material.color);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);


    for(int i = 1; i<6; i++){

        if( i % 2 == 0){ //if even

            //top strand
            world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.1f, 0.1f, -0.55f));
            world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
            VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, clam_cube->material.color);
            world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
            //lip
            world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -0.05f, 0.55f));
            world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
            VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, lip_cube->material.color);
            world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);

        }else{
            //bottom strand
            world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.1f, 0.0f, -0.55f));
            world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
            VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, clam_cube->material.color);
            world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
            //lip
            world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -0.05f, 0.55f));
            world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
            VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, lip_cube->material.color);
            world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);

        }

    }

    //last strand
    //1 setp lower
    //less long
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.1f, 0.0f, -0.55f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor - glm::vec3(0.0f, 0.0f, 0.05f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, clam_cube->material.color);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / (scale_factor - glm::vec3(0.0f, 0.0f, 0.05f)));
    //lip
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -0.05f, 0.525f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_lip);
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, lip_cube->material.color);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_lip);




}



void Renderer::DrawOneCoral(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection, const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time , glm::vec3 _color1, glm::vec3 _color2 ,glm::vec3 _color3 ){


    glm::mat4 world_transform_matrix = glm::mat4(1.0f);

    world_transform_matrix = glm::translate(world_transform_matrix, _position);
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, _rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, _scale);


    float sin_offset = cos(time/60) * 2;

    glm::mat4 secondary_transform_matrix = world_transform_matrix;

    coral_cube_3->material.color = _color1; // colour --should be the darker one
    coral_cube->material.color = _color2; // colour --should be the second darker one
    coral_cube_2->material.color = _color3; // colour for later gradient -- should be the lighter one
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
        VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, _color1);
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
        VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, _color2);
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
        VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, _color2);
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
        VisualCube::CalculateInstancedFromMatrix(world_transform_matrix,_color3);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
        world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(-sin_offset,0.0f, -15.0f));
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-radius/3, -height/3, 0.0f));

    }


}


void Renderer::DrawOneCoral2(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection, const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time , glm::vec3 _color1, glm::vec3 _color2 ,glm::vec3 _color3, float branches){


    glm::mat4 world_transform_matrix = glm::mat4(1.0f);

    world_transform_matrix = glm::translate(world_transform_matrix, _position);
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, _rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, _scale);


    float sin_offset = cos(time/30) * 2;

    glm::mat4 secondary_transform_matrix = world_transform_matrix;

    coral_cube_3->material.color = _color1; // colour --should be the darker one
    coral_cube->material.color = _color2; // colour --should be the darker one
    coral_cube_2->material.color = _color3; // colour for later gradient -- should be the lighter one
    //lip_cube->material.color = glm::vec3(255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f); // colour


    //special values
    float nb_branches = branches;
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
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, _color1);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -height/2, 0.0f));



    for(int i =0; nb_branches-1 > i; i++ ){
        //every branch will have 1+ tier 3 branches

        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, increment, 0.0f));
        world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,sin_offset/2, angle_upward));
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, height/4, 0.0f));
        world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_tier2 );
        VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, _color2);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_tier2);

        //make a tier 3 branch

        angle_upward2 = angle_upward + i*3;

        world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,sin_offset, angle_upward2));
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, height/8, 0.0f));
        world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_tier3 );
        VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, _color3);
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
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, _color2);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor_tier2);
    //tier 3
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, glm::vec3(0.0f,0.0f, angle_upward2));
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, height/8, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor_tier3 );
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, _color3);
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



void Renderer::DrawRock(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection, const glm::vec3& _eyePosition, const Shader::Material *_materialOverride,float time)
{
    glm::mat4 world_transform_matrix = glm::mat4(1.0f);

    world_transform_matrix = glm::translate(world_transform_matrix, _position);
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, _rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, _scale);

    glm::mat4 secondary_transform_matrix = world_transform_matrix;

    // rocks
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(5.0f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 0.50f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.20f, 0.25f, 1.0f));

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(2.0f, 0.0f, 0.0f));
     world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(5.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(-2.0f, 5.0f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 0.25f, 1.0f));

     world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(2.50f, 0.0f, 0.70f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(-5.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 2.0f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.20f, 1.0f, 1.0f));

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-4.50f, 0.70f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(6.0f, 4.0f, 0.5f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.20f, 0.25f, 1.0f));

     world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-3.50f, -1.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(10.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(6.0f, 2.0f, 3.5f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(6.0f, 2.0f, 3.5f));

     world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(3.50f, 2.0f, -0.80f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(-45.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(5.0f, 3.0f, 1.50f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.20f, 0.25f, 1.0f));

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(2.0f, 0.0f, 1.80f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(-75.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(5.0f, 1.0f, 1.50f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.20f, 0.25f, 1.0f));
}

void Renderer::DrawRock2(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection, const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time)
{
    glm::mat4 world_transform_matrix = glm::mat4(1.0f);

    world_transform_matrix = glm::translate(world_transform_matrix, _position);
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, _rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, _scale);


    glm::mat4 secondary_transform_matrix = world_transform_matrix;

    // rocks (skin)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.10f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 3.0f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 3.0f, 1.0f));

    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 10.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 3.0f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 3.0f, 1.0f));

    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 10.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 3.0f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 3.0f, 1.0f));

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.20f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 10.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 4.0f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 4.0f, 1.0f));

     world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.30f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 10.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 4.0f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 4.0f, 1.0f));

    //second rock

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(2.0f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 2.0f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 2.0f, 1.0f));

    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 10.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.80f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 1.80f, 1.0f));

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.20f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 10.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 2.1f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 2.1f, 1.0f));

     world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.30f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 10.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.70f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 1.70f, 1.0f));


    ////third rock

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-3.0f, 0.0f, -2.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 1.0f, 1.0f));

    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 10.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 1.0f, 1.0f));

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.20f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 10.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 1.0f, 1.0f));

     world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.30f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 10.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 1.0f, 1.0f));

     world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.50f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 10.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 1.0f, 1.0f));

    ////
    //fourth rock

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(3.0f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 4.30f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 4.30f, 1.0f));

    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 10.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 4.30f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 4.30f, 1.0f));

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.20f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 10.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 4.30f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 4.30f, 1.0f));

     world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.30f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 10.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 4.30f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 4.30f, 1.0f));

    //fifth rock
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(2.0f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 2.30f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 2.30f, 1.0f));

    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 10.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 2.30f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 2.30f, 1.0f));

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.20f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 10.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 2.30f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 2.30f, 1.0f));

     world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.30f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 10.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 2.30f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 2.30f, 1.0f));

    //sixth rock
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-2.0f, 0.0f, -1.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 2.30f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 2.30f, 1.0f));

    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 10.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 2.30f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 2.30f, 1.0f));

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.20f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 10.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 2.30f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 2.30f, 1.0f));

     world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.30f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 10.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 2.30f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 2.30f, 1.0f));

}

void Renderer::DrawRock3(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection, const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time)
{
    glm::mat4 world_transform_matrix = glm::mat4(1.0f);

    world_transform_matrix = glm::translate(world_transform_matrix, _position);
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, _rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, _scale);


    glm::mat4 secondary_transform_matrix = world_transform_matrix;

    // rock base
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(5.0f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.5f, 1.50f, 1.50f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.5f, 1.50f, 1.50f));

//second
     world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 20.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.5f, 1.50f, 6.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.5f, 1.50f, 6.0f));

     world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(1.50f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 0.0f, -30.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 2.0f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 2.0f, 1.0f));

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-0.50f, 1.50f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, -30.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(3.0f, 2.0f, 6.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 2.0f, 1.0f));

     world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, -40.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 2.0f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 2.0f, 1.0f));

    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 0.0f, 10.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 2.0f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 2.0f, 1.0f));

 world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 20.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 2.0f, 1.0f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.344f));
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f/glm::vec3(1.0f, 2.0f, 1.0f));

}
void Renderer::DrawPebble(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection, const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time)
{
    glm::mat4 world_transform_matrix = glm::mat4(1.0f);

    world_transform_matrix = glm::translate(world_transform_matrix, _position);
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix, _rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, _scale);


    glm::mat4 secondary_transform_matrix = world_transform_matrix;

    // rock base
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(5.0f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.70f, 0.20f, 0.80f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.444f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 1.0f));

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.50f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.60f, 0.90f, 0.70f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.444f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 1.0f));

     world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.50f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.50f, 0.90f, 0.50f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.444f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 1.0f));

     world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(3.0f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.50f, 0.50f, 0.50f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.444f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 1.0f));

     world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-13.0f, 0.0f, 0.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.50f, 0.50f, 0.50f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.444f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 1.0f));

 world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-3.0f, 0.0f, -2.0f));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.70f, 0.70f, 0.70f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.444f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 1.0f));

    for (int i=0;i<6;i++){
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-3.0f+i, 0.0f, -2.0f+i));
    world_transform_matrix = Transform::RotateDegrees(world_transform_matrix,  glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.70f, 0.70f, 0.70f));
    VisualCube::CalculateInstancedFromMatrix(world_transform_matrix, glm::vec3(0.444f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 1.0f));

    }


}

void Renderer::InputCallback(GLFWwindow* _window, const double _deltaTime) {
    //keyboard triggers
    if (Input::IsKeyReleased(_window, GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(_window, true);

    if (Input::IsKeyReleased(_window, GLFW_KEY_ENTER))
        SwitchScenes();

    if (Input::IsKeyReleased(_window, GLFW_KEY_X))
    {
        animateCamera = !animateCamera;
    }

    if (is_underwater)
    {
        //check if we are going out of bounds
        if (10.0f > main_camera->GetPosition().x) {
            main_camera->SetPosition(glm::vec3(20.0f, main_camera->GetPosition().y, main_camera->GetPosition().z));
        }
        if (main_camera->GetPosition().x > (main_terrain->GetGridSize() - 10.0f)) {
            main_camera->SetPosition(glm::vec3((main_terrain->GetGridSize() - 10.0f), main_camera->GetPosition().y, main_camera->GetPosition().z));
        }
        if (20.0f > main_camera->GetPosition().z) {
            main_camera->SetPosition(glm::vec3(main_camera->GetPosition().x, main_camera->GetPosition().y, 10.0f));
        }
        if (main_camera->GetPosition().z > (main_terrain->GetGridSize() - 10.0f)) {
            main_camera->SetPosition(glm::vec3(main_camera->GetPosition().x, main_camera->GetPosition().y, (main_terrain->GetGridSize() - 20.0f)));
        }
    }

    //camera translates (side to side and zoom forwards & back)
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_7) || Input::IsKeyPressed(_window, GLFW_KEY_Q))
        main_camera->OneAxisMove(Camera::Translation::UP, 0.2f);
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_9) || Input::IsKeyPressed(_window, GLFW_KEY_E))
        main_camera->OneAxisMove(Camera::Translation::DOWN, 0.2f);
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_4) || Input::IsKeyPressed(_window, GLFW_KEY_W))
        main_camera->OneAxisMove(Camera::Translation::CAMERA_FORWARD, 0.2f);
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_6) || Input::IsKeyPressed(_window, GLFW_KEY_A))
        main_camera->OneAxisMove(Camera::Translation::LEFT, 0.2f);
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_8) || Input::IsKeyPressed(_window, GLFW_KEY_S))
        main_camera->OneAxisMove(Camera::Translation::CAMERA_BACKWARD, 0.2f);
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_2) || Input::IsKeyPressed(_window, GLFW_KEY_D))
        main_camera->OneAxisMove(Camera::Translation::RIGHT, 0.2f);

    //mouse triggers
    //forwards & back in camera local coordinates
    if (Input::IsMouseButtonPressed(_window, GLFW_MOUSE_BUTTON_LEFT))
        main_camera->OneAxisMove(Camera::Translation::CAMERA_FORWARD, (float)Input::cursor_delta_y / 15.0f);

    //tilt control
    main_camera->OneAxisRotate(Camera::Rotation::POSITIVE_PITCH, -(float)Input::cursor_delta_y / 15.0f);
    //pan control
    main_camera->OneAxisRotate(Camera::Rotation::POSITIVE_YAW, -(float)Input::cursor_delta_x / 15.0f);

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

    main_camera->CubeIntersection(main_terrain->GetVertices());
}