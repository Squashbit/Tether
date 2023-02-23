#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 fragCoord;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D image;

void main() 
{
    outColor = texture(image, fragCoord);
}