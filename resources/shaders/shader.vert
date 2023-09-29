#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(set = 0, binding = 0) uniform GlobalUniform {
    mat4 viewProjection;
} global;

layout(set = 1, binding = 0) uniform ModelUniform {
    mat4 model;
    vec4 color;
} model;

layout(location = 0) out vec2 outUV;

void main() {
    gl_Position = global.viewProjection * model.model * vec4(inPosition, 1.0);
    outUV = inUV;
}