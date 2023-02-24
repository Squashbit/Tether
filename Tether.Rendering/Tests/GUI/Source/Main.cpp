#include <Tether/Tether.hpp>
#include <Tether/Common/Stopwatch.hpp>

#include <Tether/Module/Rendering/WindowUI.hpp>
#include <Tether/Module/Rendering/Vulkan/GlobalVulkan.hpp>
#include <Tether/Module/Rendering/Vulkan/VulkanCompositor.hpp>

#include <Tether/Module/Rendering/Elements/Button.hpp>

#include <iostream>
#include <vector>
#include <stdexcept>

using namespace Tether;
using namespace Rendering;

class DebugLogger : public Vulkan::DebugCallback
{
public:
	void OnDebugLog(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
	)
	{
		switch (messageSeverity)
		{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			{
				std::cout << pCallbackData->pMessage << std::endl;
			}
			break;
		}
	}
};

class RendererTestApp
{
public:
	RendererTestApp()
		:
		window(Window::Create(1280, 720, L"Text testing")),
		m_VulkanWindow(*window),
		m_Renderer(m_VulkanWindow.MakeVulkanContext()),
		m_Compositor(m_Renderer, m_VulkanWindow),
		m_WindowUI(*window, m_Renderer, m_Compositor),
		m_Button(m_WindowUI)
	{
		m_WindowUI.SetBackgroundColor(Math::Vector4f(0.02f, 0.02f, 0.02f, 1.0f));

		m_Button.SetX(100.0f);
		m_Button.SetY(100.0f);
		m_Button.SetWidth(140.0f);
		m_Button.SetHeight(80.0f);
		m_Button.SetBorderSize(0.0f);
		m_Button.SetBackgroundColor(Math::Vector4f(0.1f, 0.1f, 0.1f, 1.0f));
		m_Button.SetOnClickFunction(std::bind(&RendererTestApp::ButtonClick, this));
		m_WindowUI.AddElement(m_Button);

		window->SetVisible(true);
	}

	void Run()
	{
		window->Run();
	}

	void ButtonClick()
	{
		std::cout << "Clicked\n";
	}
private:
	Scope<Window> window;

	Vulkan::VulkanWindow m_VulkanWindow;
	Vulkan::VulkanRenderer m_Renderer;
	Vulkan::VulkanCompositor m_Compositor;

	WindowUI m_WindowUI;

	Elements::Button m_Button;
};

#if defined(_WIN32) && !defined(_DEBUG)
#include <Windows.h>
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
#else
int main()
#endif
{
	DebugLogger vulkanLogger;
	Vulkan::GlobalVulkan::Get().AddDebugMessenger(vulkanLogger);

	RendererTestApp testApp;
	testApp.Run();

	return 0;
}