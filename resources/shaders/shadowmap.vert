#version 450

layout(location = 0) in vec3 inPosition;

layout(set = 0, binding = 0) uniform GlobalUniform {
    mat4 viewProjection;
} global;

layout(set = 0, binding = 1) uniform ModelUniform {
    mat4 model;
    vec4 color;
} model;

void main()
{
    gl_Position = global.viewProjection * model.model * vec4(inPosition, 1.0);
}