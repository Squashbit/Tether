#include <Tether/Tether.hpp>
#include <Tether/Common/Stopwatch.hpp>
#include <Tether/Math/Constants.hpp>

#include <Tether/Module/Rendering/ImageLoader.hpp>
#include <Tether/Module/Rendering/Objects/Image.hpp>
#include <Tether/Module/Rendering/Vulkan/VulkanRenderer.hpp>
#include <Tether/Module/Rendering/Vulkan/VulkanCompositor.hpp>
#include <Tether/Module/Rendering/Vulkan/GlobalVulkan.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/TypeNames.hpp>

#include <iostream>
#include <vector>
#include <chrono>
#include <stdexcept>

using namespace Tether;
using namespace Rendering;
using namespace Vulkan;

class DebugLogger : public Rendering::Vulkan::DebugCallback
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
		m_Window(Window::Create(1280, 720, L"Renderer testing")),
		m_VulkanWindow(*m_Window),
		m_Renderer(m_VulkanWindow.MakeVulkanContext()),
		m_Compositor(m_Renderer, m_VulkanWindow)
	{
		ImageLoader imageLoader("Assets/Test.png");
		if (!imageLoader.Load())
			throw std::runtime_error("Failed to load image");

		testImage = m_Renderer.CreateResource<Resources::BufferedImage>(
			imageLoader.GetImageInfo());
		
		objects.resize(numObjects);
		for (size_t i = 0; i < numObjects; i++)
		{
			objects[i] = m_Renderer.CreateObject<Objects::Image>();
			Objects::Image* image = objects[i].get();
			image->SetImage(*testImage);
			
			m_Renderer.AddObject(*image);
		}

		m_Window->SetVisible(true);
	}

	void Run()
	{
		while (!m_Window->IsCloseRequested())
		{
			float delta = deltaTimer.GetElapsedSeconds();
			deltaTimer.Set();

			time += delta;
			frames++;

			if (fpsTimer.GetElapsedSeconds() >= 3.0f)
			{
				std::cout << "FPS = " << 1.0f / (time / frames) << std::endl;

				time = 0;
				frames = 0;

				fpsTimer.Set();
			}

			int windowWidth = m_Window->GetWidth();
			int windowHeight = m_Window->GetHeight();
			const float imageWidth = (1.0f / numObjects) * windowWidth;
			const float imageHeight = (1.0f / numObjects) * windowHeight;
			
			for (size_t i = 0; i < numObjects; i++)
			{
				Objects::Image* image = objects[i].get();

				float yTime = fullTime.GetElapsedSeconds() / 3;
				yTime += (numObjects - i) * 0.03f;
				
				float ypos = abs(sin(yTime * Math::PI));
				ypos *= 1 - lineSpacing;

				image->SetX((i / (float)numObjects) * windowWidth);
				image->SetY((1 - ypos - lineSpacing) * windowHeight);
				image->SetWidth(imageWidth);
				image->SetHeight(imageHeight);
			}

			m_Window->PollEvents();
			m_Compositor.RenderFrame();
		}
	}
private:
	size_t numObjects = 10;
	float lineSpacing = 0.1f;

	size_t frames = 0;
	float time = 0.0f;

	Scope<Window> m_Window;

	Vulkan::VulkanWindow m_VulkanWindow;
	Vulkan::VulkanRenderer m_Renderer;
	Vulkan::VulkanCompositor m_Compositor;

	Scope<Resources::BufferedImage> testImage;
	
	std::vector<Scope<Objects::Image>> objects;

	Stopwatch fpsTimer;
	Stopwatch deltaTimer;
	Stopwatch fullTime;
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