#pragma once

#include <Tether/Common/Defs.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
    class TETHER_EXPORT DebugCallback
    {
        friend class GlobalVulkan;
    public:
        ~DebugCallback();

        virtual void OnDebugLog(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
        )
        {}
    protected:
        bool m_IsInGlobalVulkan = false;
    };
}
