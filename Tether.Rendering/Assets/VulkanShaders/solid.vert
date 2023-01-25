#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 position;

layout(push_constant) uniform Rectangle
{
    vec2 position;
    vec2 scale;
    vec3 color;
} rect;

void main() 
{
    vec2 vertexPosition = position.xy;
    vertexPosition *= rect.scale.xy;
    vertexPosition += rect.position.xy;

    vertexPosition *= 2;
    vertexPosition -= 1;

    gl_Position = vec4(vertexPosition, 0.0, 1.0);
}