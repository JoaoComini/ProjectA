#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;

layout(set = 0, binding = 0) uniform CameraUniform {
    mat4 view;
    mat4 projection;
} camera;

layout(push_constant) uniform ModelConstant {
    mat4 model;
} constant;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;

void main() {
    gl_Position = (camera.projection * camera.view) * constant.model * vec4(inPosition, 1.0);
    outColor = inColor;
    outTexCoord = inTexCoord;
}