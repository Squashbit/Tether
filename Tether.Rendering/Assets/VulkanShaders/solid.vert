#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 fragColor;

layout(binding = 0) uniform Transform
{
    vec2 position;
    vec2 scale;
} trans;

void main() 
{
    vec2 screenSpacePosition = trans.position.xy;
    screenSpacePosition -= 1;
    screenSpacePosition *= 2;

    vec2 vertexPosition = position.xy;
    vertexPosition *= trans.scale.xy;
    vertexPosition += screenSpacePosition;

    fragColor = color;
    gl_Position = vec4(vertexPosition, 0.0, 1.0);
}