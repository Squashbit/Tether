#pragma once

#include <Tether/Module/Rendering/Common/Defs.hpp>

#include <Tether/SimpleWindow.hpp>
#include <Tether/Common/IDisposable.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
    class Instance;
    class InstanceLoader;
    class TETHER_EXPORT Surface : public IDisposable
    {
    public:
        Surface(Instance* pInstance, SimpleWindow* window);
        TETHER_NO_COPY(Surface);

        VkSurfaceKHR Get();
    protected:
        void OnDispose();
    private:
        Instance* pInstance;
        Vulkan::InstanceLoader* pLoader;

        VkSurfaceKHR surface;
    };
}
