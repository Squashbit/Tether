#include <iostream>

#include <Tether/Tether.hpp>

#include <Tether/Rendering/ImageLoader.hpp>
#include <Tether/Rendering/Vulkan/GlobalVulkan.hpp>
#include <Tether/Rendering/Vulkan/Compositor.hpp>

#include <thread>
#include <chrono>
#include <math.h>

using namespace std::literals::chrono_literals;
using namespace Tether;
using namespace Rendering;

static constexpr const size_t WINDOW_COUNT = 30;
static bool s_WindowsClosed[WINDOW_COUNT]{};

class TestWindow
{
public:
	class EventHandler : public Events::EventHandler
	{
	public:
		EventHandler(TestWindow& window, size_t windowIndex)
			:
			m_Window(window),
			m_WindowIndex(windowIndex)
		{
			if (windowIndex >= WINDOW_COUNT)
				throw std::invalid_argument("Window index larger than window count");
		}

		void OnWindowRepaint()
		{
			m_Window.Render();
		}

		void OnWindowClosing() override;
		
		TestWindow& m_Window;
		size_t m_WindowIndex = 0;
	};

	TestWindow(const Resources::BufferedImageInfo& imageInfo,
		Devices::Monitor& monitor, size_t windowIndex)
		:
		m_Window(Window::Create(700, 700, std::to_wstring(windowIndex + 1))),
		m_VulkanWindow(*m_Window),
		m_Renderer(m_VulkanWindow.MakeVulkanContext()),
		m_Compositor(m_Renderer, m_VulkanWindow),
		handler(*this, windowIndex)
	{
		int usableWidth = monitor.GetWidth() - m_Window->GetWidth();
		int usableHeight = monitor.GetHeight() - m_Window->GetHeight();

		m_Window->AddEventHandler(handler, Events::EventType::WINDOW_CLOSING);
		m_Window->AddEventHandler(handler, Events::EventType::WINDOW_REPAINT);

		m_BufferedImage = m_Renderer.CreateResource<Resources::BufferedImage>(
			imageInfo);

		m_Image = m_Renderer.CreateObject<Objects::Image>();
		m_Image->SetWidth(m_Window->GetWidth());
		m_Image->SetHeight(m_Window->GetHeight());
		m_Image->SetImage(*m_BufferedImage);

		m_Renderer.AddObject(*m_Image);

		m_Window->SetResizable(false);
		m_Window->SetRawInputEnabled(true);
		m_Window->SetVisible(true);
		m_Window->SetPosition(std::max(0, rand() % usableWidth), 
			std::max(0, rand() % usableHeight));

		Render();
	}

	void Render()
	{
		m_Compositor.RenderFrame();
	}

	bool IsCloseRequested()
	{
		return m_Window->IsCloseRequested();
	}
private:
	Scope<Window> m_Window;

	Vulkan::VulkanWindow m_VulkanWindow;
	Vulkan::VulkanRenderer m_Renderer;
	Vulkan::VulkanCompositor m_Compositor;

	Scope<Resources::BufferedImage> m_BufferedImage;
	Scope<Objects::Image> m_Image;

	EventHandler handler;
};

static std::vector<Scope<TestWindow>> windows;

void TestWindow::EventHandler::OnWindowClosing()
{
	m_Window.m_Window->SetVisible(false);
	s_WindowsClosed[m_WindowIndex] = true;

	for (size_t i = 0; i < WINDOW_COUNT; i++)
		if (!s_WindowsClosed[i])
			return;

	for (size_t i = 0; i < WINDOW_COUNT; i++)
	{
		windows[i]->m_Window->SetVisible(true);
		windows[i]->Render();
		s_WindowsClosed[i] = false;

		std::this_thread::sleep_for(25ms);
	}
}

#if defined(_WIN32) && !defined(_DEBUG)
#include <Windows.h>
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
#else
int main()
#endif
{
	Devices::Monitor monitor = Application::Get().GetMonitors()[0];
	windows.reserve(WINDOW_COUNT);

	{
		ImageLoader imageLoader("Assets/idiot.png");
		if (!imageLoader.Load())
			throw std::runtime_error("Failed to load image");

		for (size_t i = 0; i < WINDOW_COUNT; i++)
		{
			windows.push_back(std::make_unique<TestWindow>(imageLoader.GetImageInfo(),
				monitor, i));
			std::this_thread::sleep_for(15ms);
		}
	}

	Application::Get().Run();

	return 0;
}