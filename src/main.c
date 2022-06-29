#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// #define HANDMADE_MATH_NO_SSE
// #include "HandmadeMath.h"

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

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2); 

	while (!glfwWindowShouldClose(window)) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        	glfwSetWindowShouldClose(window, 1);
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	    glClear(GL_COLOR_BUFFER_BIT);

		int timeLocation = glGetUniformLocation(shaderProgram, "time");
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glUniform1f(timeLocation, glfwGetTime());

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
        glfwPollEvents();  
	}

    glfwTerminate();

	return 0;
}
