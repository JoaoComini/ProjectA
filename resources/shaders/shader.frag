#version 450

layout(location = 0) in vec2 inUV;

layout(set = 1, binding = 0) uniform ModelUniform {
    mat4 model;
    vec4 color;
} model;

layout(set = 1, binding = 1) uniform sampler2D inSampler;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = model.color * texture(inSampler, inUV);

    if (outColor.w < 0.5) {
        discard;
    }
}