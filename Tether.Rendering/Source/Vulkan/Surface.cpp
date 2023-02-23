#include <Tether/Module/Rendering/Vulkan/Surface.hpp>
#include <Tether/Module/Rendering/Vulkan/Instance.hpp>
#include <Tether/Native.hpp>

#include <stdexcept>

#ifdef __linux__
#include <Tether/Platform/X11Window.hpp>

#include <vulkan/vulkan_xlib.h>
#elif _WIN32
#include <Tether/Platform/Win32Window.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vulkan/vulkan_win32.h>
#endif

using namespace Tether::Rendering::Vulkan;
using namespace Tether;

Surface::Surface(VkInstance instance, InstanceLoader& loader, Window& window)
	:
	m_Instance(instance),
	m_Loader(loader)
{
#ifdef __linux__
	Native::X11Window& windowNative =
		(Native::X11Window&)window.GetWindowNative();

	VkXlibSurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
	createInfo.dpy = Application::Get().GetStorage()->display;
	createInfo.window = windowNative.window;
	
	PFN_vkCreateXlibSurfaceKHR func = (PFN_vkCreateXlibSurfaceKHR)
		m_Loader.vkCreateXlibSurfaceKHR;
#elif _WIN32
	Platform::Win32Window& windowNative = (Platform::Win32Window&)window;

	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = windowNative.GetHWND();
	createInfo.hinstance = windowNative.GetHINSTANCE();

	PFN_vkCreateWin32SurfaceKHR func = (PFN_vkCreateWin32SurfaceKHR)
		m_Loader.vkCreateWin32SurfaceKHR;
#endif

	if (func(m_Instance, &createInfo, nullptr,
		&m_Surface) != VK_SUCCESS)
		throw std::runtime_error("Surface creation failed");
}

Surface::~Surface()
{
	m_Loader.vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
}

VkSurfaceKHR Surface::Get()
{
	return m_Surface;
}
