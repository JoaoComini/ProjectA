#version 450

#define GAMMA 2.2

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform sampler2D gBuffer;

void main() 
{
    vec3 color = texture(gBuffer, inUV).rgb;
  
//  vec3 mapped = vec3(1.0) - exp(-color * hdr.exposure);  add a better tonemapper here

    vec3 corrected = pow(color, vec3(1.0 / GAMMA));

    outColor = vec4(corrected, 1.0);
}