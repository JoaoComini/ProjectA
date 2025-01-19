#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(set = 0, binding = 0) uniform ModelUniform {
    mat4 localToWorldMatrix;
} model;

layout(push_constant, std430) uniform LightPushConstant
{
    vec3 direction;
    float depthBias;
    float normalBias;
    mat4 viewProjection;
} light;

float Saturate(float t)
{
	return clamp(t, 0.0, 1.0);
}

vec3 ApplyShadowBias(vec3 position, vec3 normal)
{   
    // Depth bias
    position = light.direction * light.depthBias + position;

    // Normal bias
    float invNdotL = 1.0 - Saturate(dot(light.direction, normal));
    float scale = invNdotL * light.normalBias;
    position = normal * scale.xxx + position;
    
    return position;
}

void main()
{
    vec4 position = model.localToWorldMatrix * vec4(inPosition, 1.0);
    vec3 normal = mat3(model.localToWorldMatrix) * inNormal;

    vec3 biased = ApplyShadowBias(position.xyz, normal);

    gl_Position = light.viewProjection * vec4(biased, 1.0);
}