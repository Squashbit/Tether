#ifndef _TETHER_VULKAN_SURFACE_HPP
#define _TETHER_VULKAN_SURFACE_HPP

#include <Tether/SimpleWindow.hpp>
#include <Tether/Common/Defs.hpp>
#include <Tether/Common/IDisposable.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Vulkan
{
    class Instance;
    class InstanceLoader;
    class Surface : public IDisposable
    {
    public:
        Surface() {}
        TETHER_NO_COPY(Surface);

        /**
         * @brief Initializes the surface object.
         * 
         * @param pInstance A pointer to the Vulkan instance.
         * @param window The window object.
         * 
         * @returns True if initialization succeeded. Otherwise, false.
         */
        bool Init(Instance* pInstance, SimpleWindow* window);

        VkSurfaceKHR Get();
    protected:
        void OnDispose();
    private:
        Instance* pInstance;
        Vulkan::InstanceLoader* pLoader;

        VkSurfaceKHR surface;
    };
}

#endif //_TETHER_VULKAN_SURFACE_HPP
