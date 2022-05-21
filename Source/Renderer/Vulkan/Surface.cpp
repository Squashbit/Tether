#include <Tether/Renderer/Vulkan/Surface.hpp>
#include <Tether/Renderer/Vulkan/Instance.hpp>
#include <Tether/Native.hpp>

#ifdef __linux__
#include <vulkan/vulkan_xlib.h>
#elif _WIN32
#include <Windows.h>
#include <vulkan/vulkan_win32.h>
#endif

using namespace Tether::Vulkan;
using namespace Tether;

bool Surface::Init(Instance* pInstance, Tether::SimpleWindow* window)
{
    if (initialized)
        return false;
    
    this->pInstance = pInstance;

#ifdef __linux__
    VkXlibSurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    createInfo.dpy = Application::Get().GetStorage()->display;
    createInfo.window = window->GetStorage()->window;
    
    if (vkCreateXlibSurfaceKHR(pInstance->Get(), &createInfo, nullptr,
        &surface) != VK_SUCCESS)
        return false;
#elif _WIN32
    Tether::Storage::VarStorage* varStorage = 
        ((Tether::Storage::VarStorage*)window->GetStorage());

    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = varStorage->window;
    createInfo.hinstance = varStorage->hinst;
    
    if (vkCreateWin32SurfaceKHR(pInstance->Get(), &createInfo, nullptr,
        &surface) != VK_SUCCESS)
        return false;
#endif
    
    initialized = true;
    return true;
}

VkSurfaceKHR Surface::Get()
{
    return surface;
}

void Surface::OnDispose()
{
    vkDestroySurfaceKHR(pInstance->Get(), surface, nullptr);
}
