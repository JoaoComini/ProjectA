#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(set = 0, binding = 0) uniform GlobalUniform {
    mat4 model;
    mat4 viewProjection;
    vec3 cameraPosition;
} global;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;

void main() {
    outPosition = vec3(global.model * vec4(inPosition, 1.0));
    outUV = inUV;
    outNormal = mat3(global.model) * inNormal;

    gl_Position = global.viewProjection * vec4(outPosition, 1.0);
}