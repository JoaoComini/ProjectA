#version 450

layout (location = 0) in vec3 inUV;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 1) uniform samplerCube skybox;

void main() 
{
    vec3 envColor = texture(skybox, inUV).rgb;

	outColor = vec4(envColor, 1.0);
}
