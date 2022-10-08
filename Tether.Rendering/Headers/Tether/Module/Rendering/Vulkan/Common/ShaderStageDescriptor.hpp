#ifndef _TETHER_VULKAN_SHADERSTAGEDESCRIPTOR_HPP
#define _TETHER_VULKAN_SHADERSTAGEDESCRIPTOR_HPP

#include <Tether/Module/Rendering/Vulkan/ShaderModule.hpp>
#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
    struct ShaderStageDescriptor
    {
        VkShaderStageFlagBits stage;
        ShaderModule* pModule;
        const char* entrypoint;
    };
}

#endif //_TETHER_VULKAN_SHADERSTAGEDESCRIPTOR_HPP