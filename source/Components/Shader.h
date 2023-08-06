// Inspiration for this class and its functionality (shader reading from a text file & library caching) comes from an unreleased personal project

#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include "glad/glad.h" // include glad to get all the required OpenGL headers
#include "glm/vec2.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Light.h"
#include "Texture.h"

class Shader {
public:
    class Library {
    private:
        inline static std::unordered_map<std::string, uint32_t> shader_library;
        inline static std::unordered_map<std::string, std::shared_ptr<Shader>> compiled_shader_library;

    public:
        Library();

        static std::shared_ptr<Shader> CreateShader(const std::string& _vertexShaderPath, const std::string& _fragmentShaderPath);
        static std::shared_ptr<Shader> CreateShader(uint32_t _vertexShaderId, uint32_t _fragmentShaderPath);

        static uint32_t AddShader(const std::string& _name, GLenum _type, GLsizei _count, const char* _code, const GLint* _length = nullptr);
        static std::shared_ptr<Shader> AddProgram(const std::string& _name, uint32_t _vertexId, uint32_t _fragmentId);

    private:
        static std::string ReadShaderCode(const std::string& shaderCodePath);
    };

    // Describes all of a shader's properties (regardless of whether they are used or not)
    struct Material {
    public:
        std::shared_ptr<Shader> shader = std::make_shared<Shader>(0, 0, 0);

        float line_thickness = 1.0f;
        float point_size = 1.0f;

        glm::vec3 color = glm::vec3(1.0f);
        float alpha = 1.0f;

        std::shared_ptr<std::vector<Light>> lights = std::make_shared<std::vector<Light>>();

        std::shared_ptr<Texture> texture = std::make_shared<Texture>(0, "", 0, 0, 0, 0);
        float texture_influence = 0.0f;
        glm::vec2 texture_tiling = glm::vec2(1.0f);

        int shininess = 32;
    };

public:
    uint32_t program_id;
    uint32_t vertex_shader_id;
    uint32_t fragment_shader_id;

public:
    Shader(uint32_t _vertexShaderId, uint32_t _fragmentShaderId, uint32_t _programId);

    void Use() const; //activates the shader

    void SetBool(const char* _name, bool _value) const; // utility function to set a bool value
    void SetInt(const char *_name, int _value) const;  // utility function to set a int _value

    void SetFloat(const char *_name, float _value) const; // utility function to set a float _value
    void SetFloatFast(const char *_name, float _value) const; // utility function to set a flow value on an active program

    void SetVec2(const char *_name, float _valueX, float _valueY) const; // utility function to set a vector 2
    void SetVec2(const char *_name, const glm::vec2& _value) const; // utility function to set a vector 2

    // utility functions to set a vector 3
    void SetVec3(const char *_name, float _valueX, float _valueY, float _valueZ) const;
    void SetVec3(const char *_name, const glm::vec3& _value) const;

    void SetMat4(const char *_name, const glm::mat4 &_value) const; // utility function to set a matrix 4x4

    void SetTexture(const char *_name, GLint _value) const; // utility function to set a texture
    void SetModelMatrix(const glm::mat4& _transform) const; // utility function to set model matrix
    void SetViewProjectionMatrix(const glm::mat4& _transform) const; // utility function to set projection matrix

    void ApplyLightsToShader(const std::shared_ptr<std::vector<Light>> _lights) const; // utility function to set light information
};

