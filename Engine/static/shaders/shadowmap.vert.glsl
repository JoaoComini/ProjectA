#version 450

precision highp float;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(set = 0, binding = 0) uniform ModelUniform {
    mat4 localToWorldMatrix;
    mat4 viewProjection;
} model;

layout(push_constant, std430) uniform LightPushConstant
{
    vec3 direction;
    float depthBias;
    float normalBias;
} light;

float Saturate(float t)
{
	return clamp(t, 0.0, 1.0);
}

vec3 ApplyShadowBias(vec3 position, vec3 normal)
{   
    // Depth bias (move vertex away from light)
    vec3 depthBias = light.direction * light.depthBias;

    // Normal bias (pushing vertex away from the light along the surface normal direction)
    vec3 normalBias = -light.normalBias * (1.0 - Saturate(dot(normal, -light.direction))) * normal;
    
    return position + depthBias + normalBias;
}

void main()
{
    vec4 position = model.localToWorldMatrix * vec4(inPosition, 1.0);
    vec3 normal = normalize(mat3(model.localToWorldMatrix) * inNormal);

    vec3 biased = ApplyShadowBias(position.xyz, normal);

    gl_Position = model.viewProjection * vec4(biased, 1.0);
}