#version 450

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec4 inColor;

layout (push_constant) uniform PushConstants {
	mat4 transform;
} pushConstants;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec4 outColor;

void main()
{
	outUV = inUV;
	outColor = inColor;
	gl_Position = pushConstants.transform * vec4(inPos.xy, 0.0, 1.0);
}