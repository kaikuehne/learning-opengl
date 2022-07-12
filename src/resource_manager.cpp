#include "resource_manager.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "stb_image.h"

std::map<std::string, Texture> ResourceManager::textures;
std::map<std::string, Shader> ResourceManager::shaders;

Shader ResourceManager::load_shader(std::string name, const char *path_vs, const char *path_fs) {
    shaders[name] = load_shader_from_file(path_vs, path_fs);
    return shaders[name];
}

Shader ResourceManager::get_shader(std::string name) { return shaders[name]; }

Texture ResourceManager::load_texture(std::string name, const char *file, bool alpha) {
    textures[name] = load_texture_from_file(file, alpha);
    return textures[name];
}

Texture ResourceManager::get_texture(std::string name) { return textures[name]; }

void ResourceManager::clear() {
    for (auto iter : shaders) {
        glDeleteProgram(iter.second.id);
    }

    for (auto iter : textures) {
        glDeleteTextures(1, &iter.second.id);
    }
}

Shader ResourceManager::load_shader_from_file(const char *path_vs, const char *path_fs) {
    std::string vertexCode;
    std::string fragmentCode;

    std::ifstream vertexShaderFile(path_vs);
    std::ifstream fragmentShaderFile(path_fs);
    std::stringstream vShaderStream, fShaderStream;

    vShaderStream << vertexShaderFile.rdbuf();
    fShaderStream << fragmentShaderFile.rdbuf();

    vertexShaderFile.close();
    fragmentShaderFile.close();
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    Shader shader;
    shader.compile(vShaderCode, fShaderCode);

    return shader;
}

Texture ResourceManager::load_texture_from_file(const char *path, bool alpha) {
    Texture texture;

    if (alpha) {
        texture.format = GL_RGBA;
        texture.image_format = GL_RGBA;
    }

    int width, height, num_channels;
    unsigned char *data = stbi_load(path, &width, &height, &num_channels, 0);
    texture.generate(width, height, data);
    stbi_image_free(data);

    return texture;
}
