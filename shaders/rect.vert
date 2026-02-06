#version 330 core
layout(location = 0) in vec2 aPos;

uniform vec2 uScale;
uniform vec2 uPos;

void main() {
    vec2 scaled = aPos * uScale;
    gl_Position = vec4(scaled + uPos, 0.0, 1.0);
}
