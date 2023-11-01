#version 450

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform sampler2D gBuffer;

layout(set = 0, binding = 1) uniform HdrSettings {
    float exposure;
    float gamma;
} hdr;

void main() 
{
    vec3 color = texture(gBuffer, inUV).rgb;
  
    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-color * hdr.exposure);

    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / hdr.gamma));
  
    outColor = vec4(mapped, 1.0);
}