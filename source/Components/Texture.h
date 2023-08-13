#pragma once

#include <unordered_map>
#include <memory>
#include <vector>
#include "glad/glad.h"

class Texture {
public:
    class Library {
    private:
        inline static std::unordered_map<std::string, std::shared_ptr<Texture>> texture_library;

    public:
        Library();

        static std::shared_ptr<Texture> CreateTexture(const std::string& _fileLocation);
        static std::shared_ptr<Texture> CreateSequenceTexture(const std::string &_fileLocation);

    private:
        static Texture LoadTexture(const std::string &_fileLocation);
        static Texture LoadSequenceTexture(const std::string &_folderLocation);
    };

private:
    GLuint texture_id = 0;
    GLsizei texture_count = 0;

    std::string file_location;

    int width = 0, height = 0, bit_depth = 0, channels = 0;

public:
    explicit Texture(const GLuint _textureId, GLsizei _textureCount, const std::string& _folderLocation, int _width, int _height, int _bitDepth, int _channels);

    void UseSingle(GLuint _textureUnit) const;
    void UseSequence(GLuint _textureUnit) const;

    static void Clear();
};
