#include <stdio.h>
#include <stdlib.h>

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spine/spine.h>
#include <spine/Extension.h>
// clang-format on

#include "HandmadeMath.h"

#include "resource_manager.h"
#include "sprite_renderer.h"
#include "texture_loader.h"

#define ZOOM_SPEED 0.03
#define DRAG_SPEED 0.1
const int WINDOW_WIDTH = 3840 / 2;
const int WINDOW_HEIGHT = 2160 / 2;
const int WINDOW_FULLSCREEN = false;

typedef struct Vertex {
    float x, y;
    float u, v;
    spine::Color color;
} Vertex;

GLFWwindow *window;

// Setup pose data, shared by all skeletons
spine::Atlas *atlas;
spine::SkeletonData *skeletonData;
spine::AnimationStateData *animationStateData;

// 5 skeleton instances and their animation states
spine::Skeleton *skeletons[5];
spine::AnimationState *animationStates[5];
char *animationNames[] = {(char *)"walk", (char *)"run", (char *)"shot"};

spine::Vector<Vertex> vertices;
unsigned short quadIndices[] = {0, 1, 2, 2, 3, 0};

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                            const GLchar *message, const void *userParam);

enum MyBlendMode {
    BLEND_NORMAL,
    BLEND_ADDITIVE,
    BLEND_MULTIPLY,
    BLEND_SCREEN,
};

spine::SpineExtension *spine::getDefaultExtension() { return new spine::DefaultSpineExtension(); }

void spine_draw_skeleton(spine::Skeleton *skeleton);
void spine_draw_mesh(spine::Vector<Vertex> vertices, unsigned short *indices, size_t numIndices, Texture *texture,
                     MyBlendMode blendmode);

void window_setup();

void spine_setup();
void spine_draw(float dt);
void spine_dispose();

int main() {
    window_setup();
    spine_setup();

    /*
    auto sprite_shader = ResourceManager::load_shader("sprite", "..\\src\\shader.vs", "..\\src\\shader.fs");
    auto projection =
            HMM_Orthographic(0.0f, static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT), 0.0f,
            -1.0f, 1.0f); sprite_shader.use(); sprite_shader.integer("sprite", 0); sprite_shader.matrix4("projection",
            projection);

    auto *sr = new SpriteRenderer(ResourceManager::get_shader("sprite"));
    float x = 100.0;
    float y = 10.0;

    auto size = HMM_Vec2(500.0f, 500.0f);
    auto rotation_degrees = 0.0f;
    */

    float dt = 0.0f;
    float lastFrame = 0.0f;

    // Create 5 skeleton instances and animation states
    // representing 5 game objects
    for (int i = 0; i < 5; i++) {
        // Create the skeleton and put it at a random position
        spine::Skeleton *skeleton = new spine::Skeleton(skeletonData);
        skeleton->setPosition(200, 200);

        // Create the animation state and enqueue a random animation, looping
        spine::AnimationState *animationState = new spine::AnimationState(animationStateData);
        spine::Animation *animation = skeletonData->findAnimation(animationNames[0]);
        animationState->setAnimation(0, animation, 1);
    }

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        dt = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, 1);
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        spine_draw(dt);
        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // Dispose of the instance data. Normally you'd do this when
    // a game object is disposed.
    for (int i = 0; i < 5; i++) {
        delete skeletons[i];
        delete animationStates[i];
    }

    ResourceManager::clear();
    glfwTerminate();

    return 0;
}

void debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                            const GLchar *message, const void *userParam) {
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) { glViewport(0, 0, width, height); }

void window_setup() {
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

    GLFWmonitor *monitor = NULL;
    if (WINDOW_FULLSCREEN) {
        monitor = glfwGetPrimaryMonitor();
    }

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", monitor, NULL);

    if (window == NULL) {
        fprintf(stderr, "%s\n", "Failed to create GLFW window");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "%s\n", "Failed to initialize GLAD");
        glfwTerminate();
        return;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(debug_message_callback, 0);
}

void spine_setup() {
    atlas = new spine::Atlas("..\\spine-examples\\spineboy\\spineboy.atlas", new MyTextureLoader());
    if (!atlas) {
        printf("Failed to load atlas\n");
        exit(-2);
    }

    spine::SkeletonJson *json = new spine::SkeletonJson(atlas);
    skeletonData = json->readSkeletonDataFile("..\\spine-examples\\spineboy\\spineboy-pro.json");
    if (!skeletonData) {
        printf("Failed to load skeleton data\n");
        delete atlas;
        exit(-2);
    }
    delete json;

    animationStateData = new spine::AnimationStateData(skeletonData);
    animationStateData->setDefaultMix(0.5f);
    // animationStateData->setMix("walk", "run", 0.2f);
    // animationStateData->setMix("walk", "shot", 0.1f);
}

void spine_draw(float dt) {
    for (int i = 0; i < 5; i++) {
        spine::Skeleton *skeleton = skeletons[i];
        spine::AnimationState *animationState = animationStates[i];
        animationState->update(dt);
        animationState->apply(*skeleton);
        skeleton->updateWorldTransform();
        spine_draw_skeleton(skeleton);
    }
}

void spine_draw_skeleton(spine::Skeleton *skeleton) {
    for (size_t i = 0, n = skeleton->getSlots().size(); i < n; ++i) {
        spine::Slot *slot = skeleton->getDrawOrder()[i];
        spine::Attachment *attachment = slot->getAttachment();
        if (!attachment) {
            continue;
        }

        MyBlendMode engineBlendMode;
        switch (slot->getData().getBlendMode()) {
        case spine::BlendMode_Normal:
            engineBlendMode = BLEND_NORMAL;
            break;
        case spine::BlendMode_Additive:
            engineBlendMode = BLEND_ADDITIVE;
            break;
        case spine::BlendMode_Multiply:
            engineBlendMode = BLEND_MULTIPLY;
            break;
        case spine::BlendMode_Screen:
            engineBlendMode = BLEND_SCREEN;
            break;
        default:
            engineBlendMode = BLEND_NORMAL;
        }

        spine::Color skeletonColor = skeleton->getColor();
        spine::Color slotColor = slot->getColor();
        spine::Color tint(skeletonColor.r * slotColor.r, skeletonColor.g * slotColor.g, skeletonColor.b * slotColor.b,
                          skeletonColor.a * slotColor.a);

        Texture *texture = NULL;
        unsigned short *indices = NULL;
        if (attachment->getRTTI().isExactly(spine::RegionAttachment::rtti)) {
            spine::RegionAttachment *regionAttachment = (spine::RegionAttachment *)attachment;
            texture =
                (Texture *)((spine::AtlasRegion *)regionAttachment->getRendererObject())->page->getRendererObject();
            vertices.setSize(4, Vertex());
            float *test = (float *)&vertices[i];
            regionAttachment->computeWorldVertices(slot->getBone(), test, (size_t)0, (size_t)sizeof(Vertex));
            for (size_t j = 0, l = 0; j < 4; j++, l += 2) {
                Vertex &vertex = vertices[j];
                vertex.color.set(tint);
                vertex.u = regionAttachment->getUVs()[l];
                vertex.v = regionAttachment->getUVs()[l + 1];
            }
            indices = quadIndices;
        } else if (attachment->getRTTI().isExactly(spine::MeshAttachment::rtti)) {
            spine::MeshAttachment *mesh = (spine::MeshAttachment *)attachment;
            vertices.setSize(mesh->getWorldVerticesLength() / 2, Vertex());
            texture = (Texture *)((spine::AtlasRegion *)mesh->getRendererObject())->page->getRendererObject();
            size_t numVertices = mesh->getWorldVerticesLength() / 2;
            float *test = (float *)&vertices[i];
            mesh->computeWorldVertices(*slot, 0, numVertices, test, (size_t)0, (size_t)sizeof(Vertex));
            for (size_t j = 0, l = 0; j < numVertices; j++, l += 2) {
                Vertex &vertex = vertices[j];
                vertex.color.set(tint);
                vertex.u = mesh->getUVs()[l];
                vertex.v = mesh->getUVs()[l + 1];
            }
            indices = quadIndices;
        }

        spine_draw_mesh(vertices, 0, i, texture, engineBlendMode);
    }
}

void spine_draw_mesh(spine::Vector<Vertex> vertices, unsigned short *indices, size_t numIndices, Texture *texture,
                     MyBlendMode blendmode) {
    unsigned int VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)3);

    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)5);

    glBindVertexArray(0);
}

void spine_dispose() {
    delete atlas;
    delete skeletonData;
    delete animationStateData;
}
