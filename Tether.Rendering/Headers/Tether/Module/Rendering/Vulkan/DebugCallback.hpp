#pragma once

#include <Tether/Common/Defs.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
    class Context;
    class TETHER_EXPORT DebugCallback
    {
        friend Context;
    public:
        DebugCallback(Context& context);
        ~DebugCallback();

        virtual void OnDebugLog(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
        )
        {}
    protected:
        bool m_IsInContext = false;
        Context& m_Context;
    };
}
