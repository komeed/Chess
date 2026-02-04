#version 330 core
out vec4 FragColor;

uniform vec3 uColor; // RGB

void main() {
    FragColor = vec4(uColor, 1.0); // alpha = 1
}