#version 450

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec3 inNormal;

layout(set = 0, binding = 1) uniform ModelUniform {
    mat4 model;
    vec4 color;
} model;

layout(set = 0, binding = 2) uniform sampler2D inSampler;

layout(set = 0, binding = 3) uniform LightUniform {
    vec4 vector;
    vec4 color;
} light;

layout(location = 0) out vec4 outColor;

vec3 ApplyDirectionalLight(vec3 normal)
{
    normal = mat3(transpose(inverse(model.model))) * normal;

	vec3 worldToLight   = normalize(-light.vector.xyz);
	float ndotl         = clamp(dot(normal, worldToLight), 0.0, 1.0);
	return ndotl * light.color.w * light.color.rgb;
}

void main() {
    vec3 lightContribution = ApplyDirectionalLight(inNormal);

    vec4 baseColor = model.color * texture(inSampler, inUV);

    vec3 ambientColor = vec3(0.1) * baseColor.rgb;

    outColor = vec4(ambientColor + lightContribution * baseColor.rgb, baseColor.a);

    if (outColor.a < 0.5) {
        discard;
    }
}