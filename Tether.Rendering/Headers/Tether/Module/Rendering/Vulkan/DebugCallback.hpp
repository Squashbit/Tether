#ifndef _TETHER_VULKAN_DEBUGCALLBACK_HPP
#define _TETHER_VULKAN_DEBUGCALLBACK_HPP

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
    class DebugCallback
    {
    public:
        /**
         * @brief Called when any message is received from vulkan validation
         *  layers.
         * 
         * @param messageSeverity The severity of the message (wow really)
         * @param messageType uhh
         * @param pCallbackData The callback data... I guess.
         */
        virtual void OnDebugLog(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
        )
        {}
    };
}

#endif //_TETHER_VULKAN_DEBUGCALLBACK_HPP
