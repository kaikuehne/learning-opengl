#version 330 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 texCoord;

uniform sampler2D ourTexture;
uniform float time;

void main() {
	// vec2 yolo;
	// yolo.x = cos(time) + texCoord.x;
	// yolo.y = sin(time) * texCoord.y;
	FragColor = texture(ourTexture, texCoord);
}
