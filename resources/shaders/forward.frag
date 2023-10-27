#version 450

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(set = 0, binding = 1) uniform ModelUniform {
    mat4 model;
    vec4 color;
} model;

layout(set = 0, binding = 2) uniform sampler2D diffuseTexture;

layout(set = 0, binding = 3) uniform LightUniform {
    vec4 vector;
    vec4 color;
} light;

layout(set = 0, binding = 4) uniform sampler2DShadow shadowMap;

layout(set = 0, binding = 5) uniform ShadowUniform {
    mat4 viewProjection;
} shadow;

layout(location = 0) out vec4 outColor;


float CalculateShadow(vec2 offset)
{
    vec4 projected = shadow.viewProjection * vec4(inPosition.xyz, 1.0);

    projected /= projected.w;

    projected.xy = 0.5 * projected.xy + 0.5;

    return texture(shadowMap, vec3(projected.xy + offset, projected.z));
}

float PCFShadow()
{
	vec2 size = 1.0 / textureSize(shadowMap, 0);

	float shadow = 0.0;
	
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			shadow += CalculateShadow(vec2(x, y) * size);
		}
	}

	return shadow / 9.0;
}

vec3 Normal()
{
	vec3 dx		= dFdx(inPosition.xyz);
	vec3 dy		= dFdy(inPosition.xyz);
	vec3 st1    = dFdx(vec3(inUV, 0.0));
	vec3 st2    = dFdy(vec3(inUV, 0.0));
	vec3 T      = (st2.t * dx - st1.t * dy) / (st1.s * st2.t - st2.s * st1.t);
	vec3 N      = normalize(inNormal);
	T           = normalize(T - N * dot(N, T));
	vec3 B      = normalize(cross(N, T));
	mat3 TBN    = mat3(T, B, N);

#ifdef HAS_NORMAL_TEXTURE
	vec3 n = texture(normalTexture, inUV).rgb;
	return normalize(TBN * (2.0 * n - 1.0));
#else
	return normalize(TBN[2].xyz);
#endif
}

vec3 ApplyDirectionalLight()
{
    vec3 N      = normalize(inNormal);
	vec3 L      = normalize(-light.vector.xyz);
	float NDotL = clamp(dot(N, L), 0.0, 1.0);

	return NDotL * light.color.rgb * light.color.w;
}

void main() {
    vec3 lightContribution = ApplyDirectionalLight() * PCFShadow();

    vec4 baseColor = texture(diffuseTexture, inUV);

    vec3 ambientColor = vec3(0.1) * baseColor.rgb;

    outColor = vec4(ambientColor + baseColor.rgb * lightContribution, baseColor.a);

    if (outColor.a < 0.5) {
        discard;
    }
}