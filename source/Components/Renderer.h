#pragma once

#include <map>
#include <vector>

#include "Camera.h"
#include "Shader.h"
#include "Visual/VisualGrid.h"
#include "GLFW/glfw3.h"
#include "Components/Visual/VisualLine.h"
#include "Components/Visual/VisualCube.h"
#include "Components/Visual/Leaf.h"
#include "Components/Terrain/GenerateTerrain.h"
#include "Components/Visual/VisualModel.h"
#include "Components/Visual/VisualPlane.h"
#include "Screen.h"
#include "SFML/Audio/Sound.hpp"

struct ObjectProperties {
    int type;
    float x_offset;
    glm::vec3 color1;
    glm::vec3 color2;
    glm::vec3 color3;
    glm::vec3 pos;
    float scaleF;
    float height;
    float branches;
};
#include "Components/Animal/Animal.hpp"

class Renderer {
private:
    std::unique_ptr<Screen> main_screen;
    std::shared_ptr<Camera> main_camera;
    std::unique_ptr<Shader::Material> shadow_mapper_material;

    std::unique_ptr<VisualPlane> titleScreenUI;
    std::unique_ptr<VisualPlane> blackFadeUI;
    std::unique_ptr<VisualPlane> whiteFadeUI;
    std::unique_ptr<VisualPlane> pressSpaceUI;

    std::unique_ptr<VisualLine> main_x_line;
    std::unique_ptr<VisualLine> main_y_line;
    std::unique_ptr<VisualLine> main_z_line;

    std::unique_ptr<Shader::Material> rock_material;
    std::unique_ptr<VisualCube> rock_cube;

    std::shared_ptr<std::vector<Light>> lights;
    std::unique_ptr<VisualCube> main_light_cube;

    std::unique_ptr<VisualModel> main_ocean;
    std::unique_ptr<Shader::Material> ocean_material;
    std::shared_ptr<Texture> main_skybox;
    std::shared_ptr<Texture> ocean_flow_map;

    std::unique_ptr<Shader::Material> boat_material;
    std::unique_ptr<VisualModel> boat;
    std::shared_ptr<Texture> boatTexture;

    std::unique_ptr<Shader::Material> fishingRod_material;
    std::unique_ptr<VisualModel> fishingRod;
    std::shared_ptr<Texture> objectTexture;

    std::unique_ptr<Shader::Material> fishingRodTurn_material;
    std::unique_ptr<VisualModel> fishingRodTurn;

    std::unique_ptr<Shader::Material> fishWire_material;
    std::unique_ptr<VisualModel> fishWire;

    std::unique_ptr<Shader::Material> fishStatic_material;
    std::unique_ptr<VisualModel> fishStatic;

    std::unique_ptr<Shader::Material> seagull_material;
    std::unique_ptr<VisualModel> seagull;

    std::unique_ptr<Shader::Material> seagullStand_material;
    std::unique_ptr<VisualModel> seagullStand;

    std::unique_ptr<Shader::Material> skybox_material;
    std::unique_ptr<VisualModel> skybox;

    std::unique_ptr<GenerateTerrain> main_terrain;

    std::unique_ptr<VisualCube> test_cube;

    //camera bounding colume
    std::unique_ptr<VisualCube>  camera_bounding_volume;

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

    std::unique_ptr<sf::SoundBuffer> main_theme_buffer, underwater_sfx_buffer, transition_buffer, waves_buffer, fishingRod_buffer;
    std::unique_ptr<sf::Sound> main_theme, underwater_sfx, transition_sfx, waves_sfx, fishingRod_sfx;

    //item storage
    std::vector<ObjectProperties> spawn_list_Global;

    std::shared_ptr<Shader> lit_shader;
    std::unique_ptr<Shader::Material> default_material;

    std::vector<Fish> fish;
    std::vector<Shark> sharks;

    int viewport_width, viewport_height;
    float coinFlip;
    float moving_angle;
    bool is_underwater = false;
    int light_placed;

    GLuint shadow_map_fbo = 0;
    GLuint shadow_map_texture = 0;

    bool animateCamera = true;
    float cutsceneStartTime = -1;
    float underwaterStartTime = -1;
    bool cutscenePressedSpace = false;
    bool fishingRodThrown = false;

public:
    Renderer(int _initialWidth, int _initialHeight);

    void Init();
    void Render(GLFWwindow* _window, double _deltaTime);

    void ResizeCallback(GLFWwindow* _window, int _displayWidth, int _displayHeight);
    void InputCallback(GLFWwindow* _window, double _deltaTime);

    void SwitchScenes();
    void DrawIntroScene(GLFWwindow* _window, double _time, double _deltaTime);

    void CreateSpawnMap();

    float getRandomFloat(float min, float max);
    void generateAnimals();
    
    void DrawOneWeed(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time, float _x_offset, glm::vec3 _weedcolor , glm::vec3 _scale_factor);
    void DrawOneWeed2(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time, float height, glm::vec3 _weedcolor, glm::vec3 _leafcolor);
    void DrawOneClam(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time, glm::vec3 _color1, glm::vec3 _color2);
    void DrawOneCoral(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time, glm::vec3 _color1, glm::vec3 _color2 ,glm::vec3 _color3);
    void DrawOneCoral2(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time,  glm::vec3 _color1, glm::vec3 _color2 ,glm::vec3 _color3, float branches);
    void DrawOneLeaf(glm::mat4 world_transform_matrix, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride);

    void DrawRock(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection, const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time);
    void DrawRock2(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection, const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time);
    void DrawRock3(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection, const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time);
    void DrawPebble(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection, const glm::vec3& _eyePosition, const Shader::Material *_materialOverride, float time);

    void SpawnAllObjects(const Shader::Material *_material);
};

