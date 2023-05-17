#include <Tether/Rendering/Vulkan/Surface.hpp>
#include <Tether/Rendering/Vulkan/Instance.hpp>

#include <stdexcept>

#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>

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

Surface::Surface(GraphicsContext& context, Window& window)
	:
	m_Instance(context.GetInstance()),
	m_Loader(context.GetInstanceLoader())
{
#ifdef __linux__
	Platform::X11Application& app = (Platform::X11Application&)
		Application::Get();
	Platform::X11Window& windowNative =
		(Platform::X11Window&)window;

	VkXlibSurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
	createInfo.dpy = app.GetDisplay();
	createInfo.window = windowNative.GetWindowHandle();
	
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
