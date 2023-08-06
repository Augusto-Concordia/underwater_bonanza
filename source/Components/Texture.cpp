#include <iostream>
#include "Texture.h"
#include "stb_image.h"

Texture::Library::Library() {
    Texture::Library::texture_library = std::unordered_map<std::string, std::shared_ptr<Texture>>();
}

std::shared_ptr<Texture> Texture::Library::CreateTexture(const std::string &_fileLocation) {
    if (!Texture::Library::texture_library.contains(_fileLocation)) {
        Texture::Library::texture_library[_fileLocation] = std::make_shared<Texture>(Texture::Library::LoadTexture(_fileLocation));
    }

    return Texture::Library::texture_library[_fileLocation];
}

Texture Texture::Library::LoadTexture(const std::string &_fileLocation) {
    // create and bind textures
    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);

    if (!texture_id)
    {
        std::cerr << "Error::Texture -> Could not generate texture id for file: " << _fileLocation << std::endl;
    }

    glBindTexture(GL_TEXTURE_2D, texture_id);

    // set filter & wrap parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // load texture with dimension data
    int width, height, channels;
    unsigned char *data = stbi_load(_fileLocation.c_str(), &width, &height, &channels, 0);
    if (!data)
    {
        std::cerr << "Error::Texture -> Could not load texture file: " << _fileLocation << std::endl;
    }

    // upload the texture to the GPU
    GLenum format = 0;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
                 0, format, GL_UNSIGNED_BYTE, data);

    // free resources
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return Texture(texture_id, _fileLocation, width, height, 8, channels);
}

Texture::Texture(GLuint _textureId, const std::string& _fileLocation, int _width, int _height, int _bitDepth, int _channels) {
    texture_id = _textureId;
    file_location = _fileLocation;
    width = _width;
    height = _height;
    bit_depth = _bitDepth;
    channels = _channels;
}

void Texture::Use(const unsigned int _textureUnit) const {
    glActiveTexture(_textureUnit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
}

void Texture::Clear() {
    glBindTexture(GL_TEXTURE_2D, 0);
}