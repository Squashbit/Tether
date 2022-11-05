#pragma once

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
