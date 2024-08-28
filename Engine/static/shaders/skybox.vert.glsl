#version 450

layout (location = 0) in vec3 inPos;

layout(set = 0, binding = 0) uniform GlobalUniform {
    mat4 viewProjection;
} global;

layout (location = 0) out vec3 outUV;

void main() 
{
	outUV = inPos;

	gl_Position = global.viewProjection * vec4(inPos, 1.0);
	gl_Position.z = 0;
}