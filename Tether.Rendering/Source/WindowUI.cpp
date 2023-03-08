#include <Tether/Module/Rendering/WindowUI.hpp>
#include <Tether/Controls/Control.hpp>

namespace Tether::Rendering
{
	WindowUI::Repainter::Repainter(WindowUI& windowUI)
		:
		m_WindowUI(windowUI)
	{}

	void WindowUI::Repainter::OnWindowRepaint(const Events::WindowRepaintEvent& event)
	{
		m_WindowUI.Repaint(true);
	}

	void WindowUI::Repainter::OnWindowResize(const Events::WindowResizeEvent& event)
	{
		if (m_WindowUI.m_RepaintOnResize)
			m_WindowUI.Repaint();
	}

	WindowUI::WindowUI(Window& window, Renderer& renderer)
		:
		m_Window(window),
		m_Renderer(renderer)
	{}

	WindowUI::WindowUI(Window& window, Renderer& renderer, Compositor& compositor)
		:
		m_Window(window),
		m_Renderer(renderer),
		m_Compositor(&compositor)
	{
		m_Repainter.emplace(*this);
		m_Window.AddEventHandler(*m_Repainter, Events::EventType::WINDOW_REPAINT);
		m_Window.AddEventHandler(*m_Repainter, Events::EventType::WINDOW_RESIZE);
	}

	WindowUI::~WindowUI()
	{
		if (m_Compositor)
			m_Window.RemoveEventHandler(*m_Repainter);
	}

	void WindowUI::SetAutoRepaint(bool autoRepaint)
	{
		m_AutoRepaint = autoRepaint;
	}

	void WindowUI::SetRepaintOnResize(bool repaintOnResize)
	{
		m_RepaintOnResize = repaintOnResize;
	}

	void WindowUI::AddElement(Elements::Element& element)
	{
		if (element.m_IsInWindowUI)
			return;

		m_Elements.push_back(&element);

		element.OnAdd();
		element.m_IsInWindowUI = true;

		Repaint(true);
	}

	bool WindowUI::RemoveElement(Elements::Element& element)
	{
		if (!element.m_IsInWindowUI)
			return false;

		for (uint64_t i = 0; i < m_Elements.size(); i++)
			if (m_Elements[i] == &element)
			{
				m_Elements.erase(m_Elements.begin() + i);

				element.OnRemove();
				element.m_IsInWindowUI = false;

				Repaint(true);

				return true;
			}

		return false;
	}

	void WindowUI::ClearElements()
	{
		for (uint64_t i = 0; i < m_Elements.size(); i++)
			m_Elements[i]->m_IsInWindowUI = false;

		m_Elements.clear();
	}

	void WindowUI::SetBackgroundColor(Math::Vector4f backgroundColor)
	{
		if (!m_Compositor)
			return;

		m_Compositor->SetClearColor(backgroundColor);
	}

	Window& WindowUI::GetWindow()
	{
		return m_Window;
	}

	Renderer& WindowUI::GetRenderer()
	{
		return m_Renderer;
	}

	void WindowUI::Repaint(bool isAutomatic)
	{
		if (!m_Compositor || (isAutomatic && !m_AutoRepaint))
			return;
		
		m_Compositor->RenderFrame();
	}

	ScopedNoRepaint::ScopedNoRepaint(WindowUI& windowUI)
		:
		m_WindowUI(windowUI)
	{
		m_WindowUI.SetAutoRepaint(false);
	}

	ScopedNoRepaint::~ScopedNoRepaint()
	{
		m_WindowUI.SetAutoRepaint(true);
	}
}
