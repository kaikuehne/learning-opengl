#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define HANDMADE_MATH_NO_SSE
#include "HandmadeMath.h"

#define ZOOM_SPEED 0.03
#define DRAG_SPEED 0.1

char* read_file(char *filename);

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

char* read_file(char *filename) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = malloc(fsize + 1);
    fread(buffer, fsize, 1, f);
    fclose(f);

    buffer[fsize] = 0;

       fclose(f);

       return buffer;
}

void checkCompile(GLuint shader) {
    GLint status;
    char infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (status == GL_TRUE) {
        return;
    }

    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
    fprintf(stderr, "%s\n", infoLog);
    exit(-1);
}

void checkLink(GLuint shader) {
    GLint status;
    char infoLog[1024];
    glGetShaderiv(shader, GL_LINK_STATUS, &status);

    if (status == GL_TRUE) {
        return;
    }

    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
    fprintf(stderr, "%s\n", infoLog);
    exit(-1);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(1800, 1800, "LearnOpenGL", NULL, NULL);
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

    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSource = read_file("src\\shader.vs");
    if (vertexShaderSource == NULL) {
        fprintf(stderr, "%s\n", "Failed to read vertex shader source");
        exit(-1);
    }

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkCompile(vertexShader);

    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderSource = read_file("src\\shader.fs");
    if (fragmentShaderSource == NULL) {
        fprintf(stderr, "%s\n", "Failed to read fragment shader source");
        exit(-1);
    }

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkCompile(fragmentShader);

    GLuint shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkLink(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);  

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load("texture.jpg", &width, &height, &nrChannels, 0);
    if (!data) {
        fprintf(stderr, "%s\n", "Failed to load texture");
        exit(-1);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    float vertices[] = {
        // positions          // colors           // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);

    GLuint EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
     glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6* sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int transformLocation = glGetUniformLocation(shaderProgram, "transform");
    int timeLocation = glGetUniformLocation(shaderProgram, "time");

    // TODO: I tried to find the identify matrix. Is this it?
    hmm_mat4 transform = HMM_Mat4d(1.0);
    hmm_vec3 scale = { 1.0, 1.0, 1.0 };
    hmm_vec3 translate = { 0 };

    double currentFrame = 0.0;
    double lastFrame = 0.0;
    double dt = 0.0;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        dt = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, 1);
        }

        if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
            scale.X += dt * ZOOM_SPEED;
            scale.Y += dt * ZOOM_SPEED;
        }

        if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
            scale.X -= dt * ZOOM_SPEED;
            scale.Y -= dt * ZOOM_SPEED;
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            translate.Y -= dt * DRAG_SPEED;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            translate.Y += dt * DRAG_SPEED;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            translate.X -= dt * DRAG_SPEED;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            translate.X += dt * DRAG_SPEED;
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glUniform1f(timeLocation, glfwGetTime());

        // Usually it is scale, then rotation and lastly translation.
        // Why: First you want to scale the object so that the translations work
        // properly. Then you rotate the axes so the translation takes place on
        // the adjusted axes. Finally, you translate the object to its position.
        transform = HMM_MultiplyMat4(HMM_Scale(scale), HMM_Translate(translate));

        glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &transform.Elements[0][0]);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();  
    }

    glfwTerminate();

    return 0;
}
