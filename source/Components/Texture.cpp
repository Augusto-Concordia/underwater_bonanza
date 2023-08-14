#include <iostream>
#include <filesystem>
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

std::shared_ptr<Texture> Texture::Library::CreateSequenceTexture(const std::string &_folderLocation) {
    if (!Texture::Library::texture_library.contains(_folderLocation)) {
        Texture::Library::texture_library[_folderLocation] = std::make_shared<Texture>(Texture::Library::LoadSequenceTexture(_folderLocation));
    }

    return Texture::Library::texture_library[_folderLocation];
}

std::shared_ptr<Texture> Texture::Library::CreateCubemapTexture(const std::string &_folderLocation) {
    if (!Texture::Library::texture_library.contains(_folderLocation)) {
        Texture::Library::texture_library[_folderLocation] = std::make_shared<Texture>(Texture::Library::LoadCubemapTexture(_folderLocation));
    }

    return Texture::Library::texture_library[_folderLocation];
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

    return Texture(texture_id, 1, _fileLocation, width, height, 8, channels);
}

Texture Texture::Library::LoadSequenceTexture(const std::string &_folderLocation) {
    // create and bind textures
    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_id);

    if (!texture_id) {
        std::cerr << "Error::Texture -> Could not generate texture id for folder: " << _folderLocation << std::endl;
    }

    std::vector<std::string> file_names;

    for (const auto& entry : std::filesystem::directory_iterator(_folderLocation)) {
        file_names.push_back(entry.path().string());
    }

    const auto file_count = file_names.size();

    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_id);

    // set filter & wrap parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // load texture with dimension data
    int width, height, channels;
    unsigned char *data = stbi_load(file_names[0].c_str(), &width, &height, &channels, 0);
    if (!data)
    {
        std::cerr << "Error::Texture -> Could not load first texture file of sequence : " << _folderLocation << std::endl;
    }

    // upload the texture to the GPU
    GLenum format = 0;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, (GLint)format, width, height, (int)file_count, 0, format, GL_UNSIGNED_BYTE, nullptr);

    // upload the first texture to the GPU
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, 1, format, GL_UNSIGNED_BYTE, data);

    // free resources
    stbi_image_free(data);

    // goes through each found file, after the first, loads it and uploads it to the GPU
    for (int layer = 1; layer < file_count; ++layer) {
        std::cout << "Loading texture " << layer + 1 << " of " << file_count << ", with name : " << file_names[layer] << "\n";

        data = stbi_load(file_names[layer].c_str(), &width, &height, &channels, 0);

        if (!data) {
            std::cerr << "Error::Texture -> Could not load texture (" << layer << ") file of sequence : " << _folderLocation << std::endl;
        }

        // upload the texture to the GPU
        format = 0;
        if (channels == 1)
            format = GL_RED;
        else if (channels == 3)
            format = GL_RGB;
        else if (channels == 4)
            format = GL_RGBA;

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, width, height, 1, format, GL_UNSIGNED_BYTE, data);

        // free resources
        stbi_image_free(data);
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    return Texture(texture_id, file_count, _folderLocation, width, height, 8, channels);
}

Texture Texture::Library::LoadCubemapTexture(const std::string &_folderLocation) {
    // create and bind textures
    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    if (!texture_id) {
        std::cerr << "Error::Texture -> Could not generate cubemap texture id for folder: " << _folderLocation << std::endl;
    }

    std::vector<std::string> file_names;

    file_names.push_back(_folderLocation + "/" + "right.bmp");
    file_names.push_back(_folderLocation + "/" + "left.bmp");
    file_names.push_back(_folderLocation + "/" + "top.bmp");
    file_names.push_back(_folderLocation + "/" + "bottom.bmp");
    file_names.push_back(_folderLocation + "/" + "front.bmp");
    file_names.push_back(_folderLocation + "/" + "back.bmp");

    const auto file_count = file_names.size();

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    // set filter & wrap parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    int width, height, channels;

    // upload the first texture to the GPU
    for (int i = 0; i < 6; ++i) {
        std::cout << "Loading texture " << i + 1 << " of " << file_count << ", with name : " << file_names[i] << "\n";

        unsigned char *data = stbi_load(file_names[i].c_str(), &width, &height, &channels, 0);

        if (!data) {
            std::cerr << "Error::Texture -> Could not load texture (" << i << ") file of cubemap : " << _folderLocation << std::endl;
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        // free resources
        stbi_image_free(data);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return Texture(texture_id, file_count, _folderLocation, width, height, 8, channels);
}

Texture::Texture(GLuint _textureId, GLsizei _textureCount,  const std::string& _fileLocation, int _width, int _height, int _bitDepth, int _channels) {
    texture_id = _textureId;
    texture_count = _textureCount;
    file_location = _fileLocation;
    width = _width;
    height = _height;
    bit_depth = _bitDepth;
    channels = _channels;
}

void Texture::UseSingle(GLuint _textureUnit) const {
    glActiveTexture(_textureUnit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
}

void Texture::UseSequence(GLuint _textureUnit) const {
    glActiveTexture(_textureUnit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_id);
}

void Texture::UseCubemap(GLuint _textureUnit) const {
    glActiveTexture(_textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
}

void Texture::Clear() {
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
