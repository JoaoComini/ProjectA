#version 450

layout(location = 0) in vec3 inPosition;

layout(set = 0, binding = 0) uniform GlobalUniform {
    mat4 model;
    mat4 viewProjection;
    vec3 cameraPosition;
} global;

void main()
{
    gl_Position = global.viewProjection * global.model * vec4(inPosition, 1.0);
}