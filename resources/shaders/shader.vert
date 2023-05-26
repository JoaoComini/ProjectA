#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(set = 0, binding = 0) uniform GlobalUniform {
    mat4 viewProjection;
    mat4 model;
} global;

layout(location = 0) out vec2 outUV;

void main() {
    gl_Position = global.viewProjection * global.model * vec4(inPosition, 1.0);
    outUV = inUV;
}