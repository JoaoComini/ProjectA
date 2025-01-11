#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(set = 0, binding = 0) uniform CameraUniform {
    mat4 viewProjectionMatrix;
    vec3 position;
} camera;

layout(set = 0, binding = 1) uniform ModelUniform {
    mat4 localToWorldMatrix;
} model;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;

void main() {
    outPosition = vec3(model.localToWorldMatrix * vec4(inPosition, 1.0));
    outNormal = mat3(model.localToWorldMatrix) * inNormal;
    outUV = inUV;

    gl_Position = camera.viewProjectionMatrix * vec4(outPosition, 1.0);
}