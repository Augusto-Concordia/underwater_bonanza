#pragma once

#include <unordered_map>
#include <memory>
#include "glad/glad.h"

class Texture {
public:
    class Library {
    private:
        inline static std::unordered_map<std::string, std::shared_ptr<Texture>> texture_library;

    public:
        Library();

        static std::shared_ptr<Texture> CreateTexture(const std::string& _fileLocation);

    private:
        static Texture LoadTexture(const std::string &_fileLocation);
    };

private:
    GLuint texture_id = 0;
    std::string file_location = "";

    int width = 0, height = 0, bit_depth = 0, channels = 0;

public:
    explicit Texture(GLuint _textureId, const std::string& _fileLocation, int _width, int _height, int _bitDepth, int _channels);

    void Use(unsigned int _textureUnit) const;
    static void Clear();
};
