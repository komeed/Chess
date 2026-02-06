#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;

uniform vec2 uScale;
uniform vec2 uPos;

out vec2 vUV;

void main()
{
    vec2 scaled = (aPos + uPos) * uScale;
    gl_Position = vec4(scaled, 0.0, 1.0);
    vUV = aUV;
}
