#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform Rectangle
{
    vec2 position;
    vec2 scale;
    vec3 color;
} rect;

void main() 
{
    outColor = vec4(rect.color, 1.0);
}