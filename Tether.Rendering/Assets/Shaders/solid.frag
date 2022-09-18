#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 inCoord;
layout(location = 0) out vec4 fragColor;

// Set 1 is the material set
layout(set = 1, binding = 0) uniform Material
{
    // Placeholder
    float brightness;
} mat;

layout(set = 1, binding = 1) uniform sampler2D albedoSampler;

void main() 
{
    vec3 finalColor = texture(albedoSampler, inCoord.xy).xyz;
    finalColor *= mat.brightness;
    
    fragColor = vec4(finalColor.xyz, 1.0);
}