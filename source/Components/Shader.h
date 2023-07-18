#pragma once

#include "glad/glad.h" // include glad to get all the required OpenGL headers
#include "glm/vec2.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <unordered_map>

class Shader {
public:
    class Library {
    private:
        inline static std::unordered_map<const char*, uint32_t> shader_library;
        inline static std::unordered_map<const char*, std::shared_ptr<Shader>> compiled_shader_library;

    public:
        Library();

        static std::shared_ptr<Shader> CreateShader(const char* _vertexShaderPath, const char* _fragmentShaderPath);
        static std::shared_ptr<Shader> CreateShader(uint32_t _vertexShaderId, uint32_t _fragmentShaderPath);

        static uint32_t AddShader(const char* _name, GLenum _type, GLsizei _count, const char* _code, const GLint* _length = nullptr);
        static std::shared_ptr<Shader> AddProgram(const char* _name, uint32_t _vertexId, uint32_t _fragmentId);

    private:
        static std::string ReadShaderCode(const char* shaderCodePath);
    };

    struct Descriptor {
    public:
        const char* vertex_shader_path = "shaders/default.vert";
        const char* fragment_shader_path = "shaders/default.frag";

        float line_thickness = 1.0f;
        float point_size = 1.0f;

        glm::vec3 color = glm::vec3(1.0f);
        float alpha = 1.0f;

        glm::vec3 light_position = glm::vec3(0.0f);
        glm::vec3 light_color = glm::vec3(1.0f);

        float ambient_strength = 0.1f;
        float specular_strength = 0.5f;
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
    void SetVec2(const char *_name, float _valueX, float _valueY) const; // utility function to set a vector 2

    // utility functions to set a vector 3
    void SetVec3(const char *_name, float _valueX, float _valueY, float _valueZ) const;
    void SetVec3(const char *_name, const glm::vec3& _value) const;

    void SetModelMatrix(const glm::mat4& _transform) const; // utility function to set model matrix
    void SetViewProjectionMatrix(const glm::mat4& _transform) const; // utility function to set projection matrix
};

