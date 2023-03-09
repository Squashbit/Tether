#include <Tether/Tether.hpp>
#include <Tether/Common/Stopwatch.hpp>

#include <Tether/Module/Rendering/WindowUI.hpp>
#include <Tether/Module/Rendering/Vulkan/GlobalVulkan.hpp>
#include <Tether/Module/Rendering/Vulkan/VulkanCompositor.hpp>

#include <Tether/Module/Rendering/Elements/Button.hpp>
#include <Tether/Module/Rendering/Elements/Division.hpp>
#include <Tether/Module/Rendering/Elements/ProgressBar.hpp>
#include <Tether/Module/Rendering/Elements/Text.hpp>

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>

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
		m_ProgressBar(m_WindowUI),
		m_Text(m_WindowUI),
		m_TheVoidText(m_WindowUI),
		m_TextColor(0.0f, 0.0f, 0.0f, 1.0f)
	{
		m_Font = m_Renderer.CreateResource<Resources::Font>("Assets/font.ttf");
		m_Font->SetSize(24);

		m_WindowUI.SetAutoRepaint(false);
		m_WindowUI.SetRepaintOnResize(true);
		m_WindowUI.SetBackgroundColor(Math::Vector4f(0.02f, 0.02f, 0.02f, 1.0f));

		m_Button.SetX(100.0f);
		m_Button.SetY(100.0f);
		m_Button.SetWidth(140.0f);
		m_Button.SetHeight(80.0f);
		m_Button.SetColor(Math::Vector4f(1.0f));
		m_Button.SetBackgroundColor(Math::Vector4f(0.1f, 0.1f, 0.1f, 1.0f));
		m_Button.SetFont(*m_Font);
		m_Button.SetText("Button");
		m_Button.SetOnClickFunction(std::bind(&RendererTestApp::ButtonClick, this));

		m_Division.SetX(200.0f);
		m_Division.SetY(200.0f);
		m_Division.SetWidth(200.0f);
		m_Division.SetHeight(200.0f);
		m_Division.SetBackgroundColor(Math::Vector4f(0.0f, 0.0f, 0.0f, 1.0f));

		m_ProgressBar.SetX(600.0f);
		m_ProgressBar.SetY(70.0f);
		m_ProgressBar.SetWidth(500.0f);
		m_ProgressBar.SetHeight(60.0f);
		m_ProgressBar.SetBackgroundColor(Math::Vector4f(0.1f, 0.1f, 0.1f, 1.0f));
		m_ProgressBar.SetBorderColor(Math::Vector4f(0.2f, 0.2f, 0.2f, 1.0f));
		m_ProgressBar.SetBorderSize(1.0f);

		m_Text.SetX(340.0f);
		m_Text.SetY(100.0f);
		m_Text.SetText("Text");
		m_Text.SetFont(*m_Font);
		m_Text.SetColor(1.0f);

		m_TheVoidText.SetX(300.0f);
		m_TheVoidText.SetY(300.0f);
		m_TheVoidText.SetText("The void");
		m_TheVoidText.SetJustify(Elements::Text::Justify::CENTER);
		m_TheVoidText.SetFont(*m_Font);
		m_TheVoidText.SetColor(1.0f);

		m_WindowUI.AddElement(m_Division);
		m_WindowUI.AddElement(m_Text);
		m_WindowUI.AddElement(m_TheVoidText);
		m_WindowUI.AddElement(m_ProgressBar);
		m_WindowUI.AddElement(m_Button);

		m_Window->SetVisible(true);
	}

	void Run()
	{
		uint8_t channelsToIncrease = 1;
		Stopwatch channelIncreaseTimer;

		Stopwatch deltaTimer;
		while (!m_Window->IsCloseRequested())
		{
			float delta = deltaTimer.GetElapsedSeconds();
			deltaTimer.Set();

			Application::Get().PollEvents();

			m_Progress = fmodf((m_Progress + 100.0f * delta), 100.0f);
			m_ProgressBar.SetProgress(m_Progress);
			
			for (uint8_t channelIndex = 0; channelIndex < 3; channelIndex++)
			{
				float& channel = m_TextColor[channelIndex];

				if ((channelsToIncrease >> channelIndex) & 1)
					channel = std::min(channel + textColorRainbowSpeed * delta, 1.0f);
				else
					channel = std::max(channel - textColorRainbowSpeed * delta, 0.0f);
			}

			if (channelIncreaseTimer.GetElapsedSeconds() > 1.0f / textColorRainbowSpeed)
			{
				channelIncreaseTimer.Set();

				channelsToIncrease++;
				if (channelsToIncrease > 0b111)
					channelsToIncrease = 1;
			}
			
			m_Text.SetColor(m_TextColor);

			m_Compositor.RenderFrame();
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

	Scope<Resources::Font> m_Font;

	Elements::Button m_Button;
	Elements::Division m_Division;
	Elements::ProgressBar m_ProgressBar;
	Elements::Text m_Text;
	Elements::Text m_TheVoidText;

	float m_Progress = 0.0f;
	Math::Vector4f m_TextColor;
	const float textColorRainbowSpeed = 2.0f;
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