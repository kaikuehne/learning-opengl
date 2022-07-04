#include "shader.h"

#include <iostream>

Shader &Shader::use() {
    glUseProgram(this->id);
    return *this;
}

void Shader::compile(const char *vertexSource, const char *fragmentSource) {
    unsigned int sVertex, sFragment, gShader;
    sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vertexSource, NULL);
    glCompileShader(sVertex);
    this->check_compile_errors(sVertex, "VERTEX");
    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fragmentSource, NULL);
    glCompileShader(sFragment);
    this->check_compile_errors(sFragment, "FRAGMENT");
    this->id = glCreateProgram();
    glAttachShader(this->id, sVertex);
    glAttachShader(this->id, sFragment);
    glLinkProgram(this->id);
    check_compile_errors(this->id, "PROGRAM");
    glDeleteShader(sVertex);
    glDeleteShader(sFragment);
}

void Shader::integer(const char *name, int value) {
    glUniform1i(glGetUniformLocation(this->id, name), value);
}

void Shader::matrix4(const char *name, const hmm_mat4 &matrix) {
    glUniformMatrix4fv(glGetUniformLocation(this->id, name), 1, false, &matrix.Elements[0][0]);
}

void Shader::vector3f(const char *name, const hmm_vec3 &vector) {
    glUniform3fv(glGetUniformLocation(this->id, name), 1, &vector.Elements[0]);
}

void Shader::check_compile_errors(unsigned int object, std::string type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- "
                      << std::endl;
        }
    } else {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- "
                      << std::endl;
        }
    }
}