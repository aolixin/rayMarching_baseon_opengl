#version 330 core
layout(location = 0) in vec2 position;

out vec3 fragColor;
void main() {
    gl_Position = vec4(position, 0.0, 1.0);
}