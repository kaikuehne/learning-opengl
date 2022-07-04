#include "sprite_renderer.h"

SpriteRenderer::SpriteRenderer(Shader shader) {
    this->shader = shader;
    this->initRenderData();
}

SpriteRenderer::~SpriteRenderer() {
    glDeleteVertexArrays(1, &this->quadVAO);
}

void SpriteRenderer::draw(Texture &texture, hmm_vec2 position, hmm_vec2 size, float rotate, hmm_vec3 color) {
    this->shader.use();

    hmm_mat4 model = HMM_Mat4d(1.0);
    model = HMM_MultiplyMat4(model, HMM_Translate(HMM_Vec3(position.X, position.Y, 0.0f)));
    model = HMM_MultiplyMat4(model, HMM_Translate(HMM_Vec3(0.5f * size.X, 0.5f * size.Y, 0.0f)));
    model = HMM_MultiplyMat4(model, HMM_Rotate(HMM_ToRadians(rotate), HMM_Vec3(0.0f, 0.0f, 1.0f)));
    model = HMM_MultiplyMat4(model, HMM_Translate(HMM_Vec3(-0.5f * size.X, -0.5f * size.Y, 0.0f)));
    model = HMM_MultiplyMat4(model, HMM_Scale(HMM_Vec3(size.X, size.Y, 1.0f)));

    this->shader.matrix4("model", model);

    // render textured quad
    this->shader.vector3f("spriteColor", color);

    glActiveTexture(GL_TEXTURE0);
    texture.bind();

    // glDrawElements(GL_TRIANGLES, 4, GL_UNSIGNED_INT, 0);

    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void SpriteRenderer::initRenderData() {
    unsigned int VBO;
    float vertices[] = {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
