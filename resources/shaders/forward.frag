#version 450

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

layout(set = 0, binding = 4) uniform LightUniform {
    vec4 vector;
    vec4 color;
} light;

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

float CalculateShadow(vec2 offset)
{
    vec4 projected = shadow.viewProjection * vec4(inPosition, 1.0);

    projected /= projected.w;

    projected.xy = 0.5 * projected.xy + 0.5;

    return texture(shadowMap, vec3(projected.xy + offset, projected.z));
}

float PCFShadow()
{
	vec2 size = 1.5 / textureSize(shadowMap, 0);

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

vec3 ApplyDirectionalLight(vec3 normal)
{
    vec3 N      = normal;
	vec3 L      = normalize(-light.vector.xyz);
	float NDotL = clamp(dot(N, L), 0.0, 1.0);

	return NDotL * light.color.rgb * light.color.w;
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
    return texture(albedoTexture, inUV);
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
	vec3 F0 = vec3(0.04);
	float F90 = Saturate(50.0 * F0.r);

	vec3 N = Normal();
	vec3 V = normalize(global.cameraPosition - inPosition);
	vec3 L = normalize(-light.vector.xyz);

	vec3 H = normalize(V + L);

	float NdotV = Saturate(dot(N, V));
	float LdotH = Saturate(dot(L, H));
	float NdotH = Saturate(dot(N, H));
	float NdotL = Saturate(dot(N, L));

	vec4 albedo = Albedo();
	float metallic = Metallic();
	float roughness = Roughness();

	vec3 diffuse = albedo.rgb * (1.0 - metallic);

	vec3  F   = F_Schlick(F0, F90, LdotH);
	float Vis = V_SmithGGXCorrelated(NdotV, NdotL, roughness);
	float D   = D_GGX(NdotH, roughness);
	vec3  Fr  = F * D * Vis;

	float Fd = Fr_DisneyDiffuse(NdotV, NdotL, LdotH, roughness);

	vec3 Lo = ApplyDirectionalLight(N) * PCFShadow() * (diffuse * (vec3(1.0) - F) * Fd + Fr);

    vec3 ambientColor = vec3(0.10) * albedo.rgb;

	outColor = vec4(ambientColor + Lo, albedo.a);

    if (outColor.a < 0.5) {
        discard;
    }
}