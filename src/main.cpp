#include <stdio.h>
#include <stdlib.h>

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include "HandmadeMath.h"

#include "resource_manager.h"
#include "sprite_renderer.h"

#define ZOOM_SPEED 0.03
#define DRAG_SPEED 0.1
const int WINDOW_WIDTH = 3840 / 2;
const int WINDOW_HEIGHT = 2160 / 2;
const int WINDOW_FULLSCREEN = false;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void debug_message_callback(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length,
                            const GLchar *message, const void *userParam);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 16);
    glfwWindowHint(GLFW_REFRESH_RATE, 60);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
    glfwSwapInterval(1);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window;
    if (WINDOW_FULLSCREEN) {
        window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL",
                                  glfwGetPrimaryMonitor(), NULL);
    } else {
        window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL",
                                  NULL, NULL);
    }

    if (window == NULL) {
        fprintf(stderr, "%s\n", "Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "%s\n", "Failed to initialize GLAD");
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(debug_message_callback, 0);

    auto sprite = ResourceManager::load_shader("sprite", "..\\src\\shader.vs",
                                               "..\\src\\shader.fs");
    auto tapir = ResourceManager::load_texture("tapir", "..\\tapir.png", false);

    auto projection =
        HMM_Orthographic(0.0f, static_cast<float>(WINDOW_WIDTH),
                         static_cast<float>(WINDOW_HEIGHT), 0.0f, -1.0f, 1.0f);
    sprite.use();
    sprite.integer("sprite", 0);
    sprite.matrix4("projection", projection);

    auto *sr = new SpriteRenderer(ResourceManager::get_shader("sprite"));
    float x = 100.0;
    float y = 10.0;

    auto size = HMM_Vec2(500.0f, 500.0f);
    auto rotation_degrees = 0.0f;

    float dt = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        dt = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, 1);
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        rotation_degrees += 500 * dt;
        y += 500 * sin(glfwGetTime() + 100) * dt;
        sr->draw(tapir, HMM_Vec2(x, y), size);
        y += 300;
        sr->draw(tapir, HMM_Vec2(x + 700.0f, y + 500.0f),
                 HMM_Vec2(size.X - 100, size.Y - 100), rotation_degrees);
        y -= 300;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ResourceManager::clear();
    glfwTerminate();

    return 0;
}

void debug_message_callback(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length,
                            const GLchar *message, const void *userParam) {
    fprintf(stderr,
            "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type,
            severity, message);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}
