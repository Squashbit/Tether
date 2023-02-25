#include <Tether/Tether.hpp>
#include <Tether/Common/Stopwatch.hpp>

#include <Tether/Module/Rendering/WindowUI.hpp>
#include <Tether/Module/Rendering/Vulkan/GlobalVulkan.hpp>
#include <Tether/Module/Rendering/Vulkan/VulkanCompositor.hpp>

#include <Tether/Module/Rendering/Elements/Button.hpp>
#include <Tether/Module/Rendering/Elements/Division.hpp>
#include <Tether/Module/Rendering/Elements/ProgressBar.hpp>

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>

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
		m_Window(Window::Create(1280, 720, L"GUI testing")),
		m_VulkanWindow(*m_Window),
		m_Renderer(m_VulkanWindow.MakeVulkanContext()),
		m_Compositor(m_Renderer, m_VulkanWindow),
		m_WindowUI(*m_Window, m_Renderer, m_Compositor),
		m_Button(m_WindowUI),
		m_Division(m_WindowUI),
		m_ProgressBar(m_WindowUI)
	{
		m_WindowUI.SetAutoRepaint(false);
		m_WindowUI.SetBackgroundColor(Math::Vector4f(0.02f, 0.02f, 0.02f, 1.0f));

		m_Button.SetX(100.0f);
		m_Button.SetY(100.0f);
		m_Button.SetWidth(140.0f);
		m_Button.SetHeight(80.0f);
		m_Button.SetBackgroundColor(Math::Vector4f(0.1f, 0.1f, 0.1f, 1.0f));
		m_Button.SetOnClickFunction(std::bind(&RendererTestApp::ButtonClick, this));

		m_Division.SetX(200.0f);
		m_Division.SetY(200.0f);
		m_Division.SetWidth(200.0f);
		m_Division.SetHeight(200.0f);
		m_Division.SetBackgroundColor(Math::Vector4f(1.0f));

		m_ProgressBar.SetX(600.0f);
		m_ProgressBar.SetY(70.0f);
		m_ProgressBar.SetWidth(500.0f);
		m_ProgressBar.SetHeight(60.0f);
		m_ProgressBar.SetBackgroundColor(Math::Vector4f(0.1f, 0.1f, 0.1f, 1.0f));
		m_ProgressBar.SetBorderColor(Math::Vector4f(0.2f, 0.2f, 0.2f, 1.0f));
		m_ProgressBar.SetBorderSize(1.0f);

		m_WindowUI.AddElement(m_Division);
		m_WindowUI.AddElement(m_ProgressBar);
		m_WindowUI.AddElement(m_Button);

		m_Window->SetVisible(true);
	}

	void Run()
	{
		Stopwatch deltaTimer;
		while (!m_Window->IsCloseRequested())
		{
			float delta = deltaTimer.GetElapsedSeconds();
			deltaTimer.Set();

			m_Window->PollEvents();
			m_Compositor.RenderFrame();

			m_Progress = std::fmodf((m_Progress + 100.0f * delta), 100.0f);
			m_ProgressBar.SetProgress(m_Progress);
		}
	}

	void ButtonClick()
	{
		float randX = (float)rand() / RAND_MAX;
		float randY = (float)rand() / RAND_MAX;

		m_Button.SetX(randX * std::max(m_Window->GetWidth() - m_Button.GetWidth(), 0.0f));
		m_Button.SetY(randY * std::max(m_Window->GetHeight() - m_Button.GetHeight(), 0.0f));
	}
private:
	Scope<Window> m_Window;

	Vulkan::VulkanWindow m_VulkanWindow;
	Vulkan::VulkanRenderer m_Renderer;
	Vulkan::VulkanCompositor m_Compositor;

	WindowUI m_WindowUI;

	Elements::Button m_Button;
	Elements::Division m_Division;
	Elements::ProgressBar m_ProgressBar;

	float m_Progress = 0.0f;
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