#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 fragCoord;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform TextInfo
{
    vec2 position;
    vec2 scale;
    vec4 color;
    ivec2 windowSize;
} text;

layout(binding = 0) uniform sampler2D image;

void main() 
{
    outColor = text.color * texture(image, fragCoord).r;
}