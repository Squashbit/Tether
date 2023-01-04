#pragma once

#include <Tether/SimpleWindow.hpp>
#include <Tether/Module/Rendering/Common/Defs.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
    class Instance;
    class InstanceLoader;
    class TETHER_EXPORT Surface
    {
    public:
        Surface(Instance* pInstance, SimpleWindow* window);
        ~Surface();
        TETHER_NO_COPY(Surface);

        VkSurfaceKHR Get();
    private:
        Instance* pInstance;
        Vulkan::InstanceLoader* pLoader;

        VkSurfaceKHR surface;
    };
}
