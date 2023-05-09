#version 450

layout(push_constant) uniform GlobalUniform {
    mat4 mvp;
} globals;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = globals.mvp * vec4(inPosition, 1.0);
    fragColor = inColor;
}