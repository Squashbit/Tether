#include <Tether/Tether.hpp>

#include <Tether/Module/Rendering/Vulkan/Context.hpp>
#include <Tether/Module/Rendering/Vulkan/Renderer.hpp>

#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_vulkan.h>

#include <iostream>
#include <vector>
#include <stdexcept>

using namespace Tether;
using namespace Rendering;
using namespace Vulkan;

class DebugLogger : public Vulkan::DebugCallback
{
public:
	DebugLogger(Context& context)
		:
		Vulkan::DebugCallback(context)
	{}

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
		m_Window(Window::Create(1280, 720, L"Text testing")),
		m_Context(true),
		m_VulkanLogger(m_Context),
		m_VulkanWindow(m_Context, *m_Window),
		m_Renderer(m_Context, m_VulkanWindow)
	{
		ImGui_ImplWin32_Init();

		uint32_t imageCount = m_Renderer.GetSwapchainImageCount();

		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = m_Context.instance;
		init_info.PhysicalDevice = m_Context.physicalDevice;
		init_info.Device = m_Context.device;
		init_info.QueueFamily = m_Context.indices.graphicsFamilyIndex;
		init_info.Queue = m_Context.queue;
		init_info.DescriptorPool = m_Context.p;
		init_info.Subpass = 0;
		init_info.MinImageCount = imageCount;
		init_info.ImageCount = imageCount;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		ImGui_ImplVulkan_Init(&init_info, m_VulkanWindow.renderPass);

		m_Window->SetVisible(true);
	}

	void Run()
	{
		while (!m_Window->IsCloseRequested())
		{
			m_Window->PollEvents();
			m_Renderer.RenderFrame();
		}
	}
private:
	Scope<Window> m_Window;

	Context m_Context;
	DebugLogger m_VulkanLogger;

	Vulkan::VulkanWindow m_VulkanWindow;
	Vulkan::Renderer m_Renderer;

	VkPipelineCache pipelineCache;
};

#if defined(_WIN32) && !defined(_DEBUG)
#include <Windows.h>
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
#else
int main()
#endif
{
	RendererTestApp testApp;
	testApp.Run();

	return 0;
}