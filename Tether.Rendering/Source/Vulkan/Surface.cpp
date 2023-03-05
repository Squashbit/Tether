#include <Tether/Module/Rendering/Vulkan/Surface.hpp>
#include <Tether/Module/Rendering/Vulkan/Instance.hpp>

#include <stdexcept>

#ifdef __linux__
#include <vulkan/vulkan_xlib.h>

#include <Tether/Platform/X11Window.hpp>
#elif _WIN32

#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vulkan/vulkan_win32.h>

#include <Tether/Platform/Win32Window.hpp>
#endif

using namespace Tether::Rendering::Vulkan;
using namespace Tether;

Surface::Surface(VulkanContext& context, Window& window)
	:
	m_Instance(context.instance),
	m_Loader(context.instanceLoader)
{
#ifdef __linux__
	Native::X11Window& windowNative =
		(Native::X11Window&)window;

	VkXlibSurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
	createInfo.dpy = (X11Application&)Application::Get().GetDisplay();
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
