#version 450

precision highp float;
precision highp sampler2DShadow;

#define GAMMA 2.2

#define MAX_LIGHT_COUNT 32
#define DIRECTIONAL_LIGHT 1.0
#define POINT_LIGHT 0.0

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUniform {
    mat4 model;
    mat4 viewProjection;
    vec3 cameraPosition;
} global;

#ifdef HAS_ALBEDO_TEXTURE
layout(set = 0, binding = 1) uniform sampler2D albedoTexture;
#endif

#ifdef HAS_NORMAL_TEXTURE
layout(set = 0, binding = 2) uniform sampler2D normalTexture;
#endif

#ifdef HAS_METALLIC_ROUGHNESS_TEXTURE
layout(set = 0, binding = 3) uniform sampler2D metallicRoughnessTexture;
#endif

struct Light
{
    vec4 vector;
    vec4 color;
};

layout(set = 0, binding = 4) uniform LightUniform {
    Light array[MAX_LIGHT_COUNT];
	int count;
} lights;

layout(set = 0, binding = 5) uniform sampler2DShadow shadowMap;

layout(set = 0, binding = 6) uniform ShadowUniform {
    mat4 viewProjection;
} shadow;

layout(push_constant, std430) uniform PbrPushConstant
{
	vec4  albedoColor;
	float metallicFactor;
	float roughnessFactor;
} pbr;

// Shadow -----------------------------------------------------------

float CalculateShadow()
{
    vec4 projected = shadow.viewProjection * vec4(inPosition, 1.0);

    projected /= projected.w;

    projected.xy = 0.5 * projected.xy + 0.5;

    return texture(shadowMap, vec3(projected.xy, projected.z));
}

// Lightning --------------------------------------------------------

vec3 ApplyDirectionalLight(vec3 normal, int index)
{
    vec3 N      = normal;
	vec3 L      = normalize(-lights.array[index].vector.xyz);
	float NDotL = clamp(dot(N, L), 0.0, 1.0);

	return NDotL * lights.array[index].color.rgb * lights.array[index].color.w;
}

float Sqr(float x)
{
	return x * x;
}

// TODO: parameterize intensity and falloff on PointLight component

float Attenuate(float dist, float radius, float intensity, float falloff)
{
	float s = dist / radius;

	if (s >= 1.0)
	{
		return 0.0;
	}

	float s2 = Sqr(s);

	return intensity * Sqr(1 - s2) / (1 + falloff * s);
}

vec3 ApplyPointLight(vec3 normal, int index)
{
    vec3 N      = normal;
	vec3 L      = lights.array[index].vector.xyz - inPosition;

	float dist = length(L);

	float att = Attenuate(dist, lights.array[index].color.w, 1, 1);

	L = normalize(L);
	float NDotL = clamp(dot(N, L), 0.0, 1.0);

	return NDotL * lights.array[index].color.rgb * att;
}


// PBR --------------------------------------------------------------

const float PI = 3.14159265359;

// [0] Frensel Schlick
vec3 F_Schlick(vec3 f0, float f90, float u)
{
	return f0 + (f90 - f0) * pow(1.0 - u, 5.0);
}

// [0] Diffuse Term
float Fr_DisneyDiffuse(float NdotV, float NdotL, float LdotH, float roughness)
{
	float E_bias        = 0.0 * (1.0 - roughness) + 0.5 * roughness;
	float E_factor      = 1.0 * (1.0 - roughness) + (1.0 / 1.51) * roughness;
	float fd90          = E_bias + 2.0 * LdotH * LdotH * roughness;
	vec3  f0            = vec3(1.0);
	float light_scatter = F_Schlick(f0, fd90, NdotL).r;
	float view_scatter  = F_Schlick(f0, fd90, NdotV).r;
	return light_scatter * view_scatter * E_factor;
}

// [0] Specular Microfacet Model
float V_SmithGGXCorrelated(float NdotV, float NdotL, float roughness)
{
	float alphaRoughnessSq = roughness * roughness;

	float GGXV = NdotL * sqrt(NdotV * NdotV * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);
	float GGXL = NdotV * sqrt(NdotL * NdotL * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);

	float GGX = GGXV + GGXL;
	if (GGX > 0.0)
	{
		return 0.5 / GGX;
	}
	return 0.0;
}

// [0] GGX Normal Distribution Function
float D_GGX(float NdotH, float roughness)
{
	float alphaRoughnessSq = roughness * roughness;
	float f                = (NdotH * alphaRoughnessSq - NdotH) * NdotH + 1.0;
	return alphaRoughnessSq / (PI * f * f);
}

float Saturate(float t)
{
	return clamp(t, 0.0, 1.0);
}

vec4 Albedo()
{
	#ifdef HAS_ALBEDO_TEXTURE
	vec4 albedo = texture(albedoTexture, inUV);
    return vec4(pow(albedo.rgb, vec3(GAMMA)), albedo.a);
	#else
	return pbr.albedoColor;
	#endif
}

vec3 Normal()
{
	vec3 dx		= dFdx(inPosition);
	vec3 dy		= dFdy(inPosition);
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

float Metallic()
{
	#ifdef HAS_METALLIC_ROUGHNESS_TEXTURE
    return Saturate(texture(metallicRoughnessTexture, inUV).b);
	#else
	return pbr.metallicFactor;
	#endif
}

float Roughness()
{
	#ifdef HAS_METALLIC_ROUGHNESS_TEXTURE
    return Saturate(texture(metallicRoughnessTexture, inUV).g);
	#else
	return pbr.roughnessFactor;
	#endif
}

void main()
{
	vec4 albedo = Albedo();

	if (albedo.a < 0.5) {
        discard;
    }

	float metallic = Metallic();
	float roughness = Roughness();

	vec3 F0 = vec3(0.04);
	float F90 = Saturate(50.0 * F0.r);

	vec3 N = Normal();
	vec3 V = normalize(global.cameraPosition - inPosition);
	float NdotV = Saturate(dot(N, V));

	vec3 diffuse = albedo.rgb * (1.0 - metallic);

	vec3 Lo = vec3(0.0);
	for(int i = 0; i < lights.count; i++)
	{
		vec3 L = vec3(0.0);

		if (lights.array[i].vector.w == DIRECTIONAL_LIGHT)
		{
			L = normalize(-lights.array[i].vector.xyz);
		}
		else
		{
			L = normalize(lights.array[i].vector.xyz - inPosition);
		}

		vec3 H = normalize(V + L);

		float LdotH = Saturate(dot(L, H));
		float NdotH = Saturate(dot(N, H));
		float NdotL = Saturate(dot(N, L));

		vec3  F   = F_Schlick(F0, F90, LdotH);
		float Vis = V_SmithGGXCorrelated(NdotV, NdotL, roughness);
		float D   = D_GGX(NdotH, roughness);
		vec3  Fr  = F * D * Vis;

		float Fd = Fr_DisneyDiffuse(NdotV, NdotL, LdotH, roughness);

		if (lights.array[i].vector.w == DIRECTIONAL_LIGHT)
		{
			Lo += ApplyDirectionalLight(N, i) * (diffuse * (vec3(1.0) - F) * Fd + Fr);

			if (i == 0)
			{
				Lo *= CalculateShadow();
			}
		}
		else
		{
			Lo += ApplyPointLight(N, i) * (diffuse * (vec3(1.0) - F) * Fd + Fr);
		}
	}

    vec3 ambientColor = vec3(0.0) * albedo.rgb;

	vec3 finalColor = ambientColor + Lo;

	outColor = vec4(finalColor, albedo.a);
}