#include "Shader.h"

Shader::Shader(uint32_t _vertexShaderId, uint32_t _fragmentShaderId, uint32_t _programId) {
    vertex_shader_id = _vertexShaderId;
    fragment_shader_id = _fragmentShaderId;
    program_id = _programId;
}

void Shader::Use() const {
    glUseProgram(program_id);
}

void Shader::SetBool(const char *_name, bool _value) const {
    glProgramUniform1ui(program_id, glGetUniformLocation(program_id, _name), (int) _value);
}

void Shader::SetInt(const char *_name, int _value) const {
    glProgramUniform1i(program_id, glGetUniformLocation(program_id, _name), _value);
}

void Shader::SetFloat(const char *_name, float _value) const {
    glProgramUniform1f(program_id, glGetUniformLocation(program_id, _name), _value);
}

void Shader::SetVec2(const char *_name, float _valueX, float _valueY) const {
    glProgramUniform2f(program_id, glGetUniformLocation(program_id, _name), _valueX, _valueY);
}

void Shader::SetVec3(const char *_name, float _valueX, float _valueY, float _valueZ) const {
    glProgramUniform3f(program_id, glGetUniformLocation(program_id, _name), _valueX, _valueY, _valueZ);
}

void Shader::SetVec3(const char *_name, const glm::vec3& _value) const {
    glProgramUniform3f(program_id, glGetUniformLocation(program_id, _name), _value.x, _value.y, _value.z);
}

void Shader::SetTexture(const char *_name, GLint _value) const {
    SetInt(_name, _value);
}

void Shader::SetMat4(const char *_name, const glm::mat4 &_value) const {
    glProgramUniformMatrix4fv(program_id, glGetUniformLocation(program_id, _name), 1, GL_FALSE, glm::value_ptr(_value));
}

void Shader::SetModelMatrix(const glm::mat4 &_transform) const {
    glProgramUniformMatrix4fv(program_id, glGetUniformLocation(program_id, "u_model_transform"), 1, GL_FALSE, glm::value_ptr(_transform));
}

void Shader::SetViewProjectionMatrix(const glm::mat4 &_transform) const {
    glProgramUniformMatrix4fv(program_id, glGetUniformLocation(program_id, "u_view_projection"), 1, GL_FALSE, glm::value_ptr(_transform));
}

Shader::Library::Library() {
    Shader::Library::shader_library = std::unordered_map<std::string, uint32_t>();
    Shader::Library::compiled_shader_library = std::unordered_map<std::string, std::shared_ptr<Shader>>();
}

std::shared_ptr<Shader> Shader::Library::CreateShader(const std::string& _vertexShaderPath, const std::string& _fragmentShaderPath) {
    std::string shaderCode;
    uint32_t vertex_id;
    uint32_t fragment_id;
    std::shared_ptr<Shader> compiled_shader;

    if (Shader::Library::shader_library.contains(_vertexShaderPath)) {
        vertex_id = Shader::Library::shader_library[_vertexShaderPath];
    } else {
        shaderCode = Shader::Library::ReadShaderCode(_vertexShaderPath);

        vertex_id = Shader::Library::AddShader(_vertexShaderPath, GL_VERTEX_SHADER, 1, shaderCode.c_str());
    }

    if (Shader::Library::shader_library.contains(_fragmentShaderPath)) {
        fragment_id = Shader::Library::shader_library[_fragmentShaderPath];
    } else {
        shaderCode = Shader::Library::ReadShaderCode(_fragmentShaderPath);

        fragment_id = Shader::Library::AddShader(_fragmentShaderPath, GL_FRAGMENT_SHADER, 1, shaderCode.c_str());
    }

    auto shader_name = std::to_string(vertex_id).append("-").append(std::to_string(fragment_id));

    if (Shader::Library::compiled_shader_library.contains(shader_name)) {
        compiled_shader = Shader::Library::compiled_shader_library[shader_name];
    } else {
        compiled_shader = Shader::Library::AddProgram(shader_name, vertex_id, fragment_id);
    }

    return compiled_shader;
}

std::shared_ptr<Shader> Shader::Library::CreateShader(uint32_t _vertexShaderId, uint32_t _fragmentShaderId) {
    auto vertexShaderIdCString = std::to_string(_vertexShaderId);
    auto fragmentShaderIdCString = std::to_string(_fragmentShaderId);

    std::shared_ptr<Shader> compiled_shader = AddProgram(
            std::to_string(_vertexShaderId).append("-").append(std::to_string(_fragmentShaderId)), _vertexShaderId,
            _fragmentShaderId);

    return compiled_shader;
}

uint32_t
Shader::Library::AddShader(const std::string& _name, GLenum _type, GLsizei _count, const char* _code, const GLint* _length) {
    uint32_t shader_id;
    int success;
    char log[512];

    shader_id = glCreateShader(_type);
    glShaderSource(shader_id, _count, &_code, _length);
    glCompileShader(shader_id);

    //error printing, if any
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader_id, 512, nullptr, log);

        std::cout << "ERROR::SHADER::" << ((_type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT")
                  << "::COMPILATION_FAILED -> (" << _name << ") " << log << std::endl;
    }

    Shader::Library::shader_library[_name] = shader_id;

    return shader_id;
}

std::shared_ptr<Shader> Shader::Library::AddProgram(const std::string& _name, uint32_t _vertexId, uint32_t _fragmentId) {
    int program_id;
    int success;
    char log[512];

    program_id = glCreateProgram();
    glAttachShader(program_id, _vertexId);
    glAttachShader(program_id, _fragmentId);
    glLinkProgram(program_id);

    //error printing, if any
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(program_id, 512, nullptr, log);

        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED -> (" << _name << ") " << log << std::endl;
    }

    std::shared_ptr<Shader> compiled_shader = std::make_shared<Shader>(_vertexId, _fragmentId, program_id);

    Shader::Library::compiled_shader_library[_name] = compiled_shader;

    return compiled_shader;
}

std::string Shader::Library::ReadShaderCode(const std::string& _shaderCodePath) {
    std::string shaderCodeString; //actual shader code
    std::ifstream shaderFile; //file handler

    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        //opens files
        shaderFile.open(_shaderCodePath);

        std::stringstream shaderStream;

        //reads files buffers into streams
        shaderStream << shaderFile.rdbuf();

        //closes file handlers
        shaderFile.close();

        shaderCodeString = shaderStream.str();
    } catch (std::ifstream::failure &e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ -> " << e.what() << std::endl << "Path: "
                  << _shaderCodePath << std::endl;
        return {};
    }

    return shaderCodeString;
}
