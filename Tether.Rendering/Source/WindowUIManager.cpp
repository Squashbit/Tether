#include <Tether/Rendering/WindowUI.hpp>
#include <Tether/Controls/Control.hpp>

namespace Tether::Rendering
{
	WindowUIManager::Repainter::Repainter(WindowUIManager& windowUI)
		:
		m_WindowUI(windowUI)
	{}

	void WindowUIManager::Repainter::OnWindowRepaint()
	{
		m_WindowUI.Repaint(true);
	}

	void WindowUIManager::Repainter::OnWindowResize(const Events::WindowResizeEvent& event)
	{
		m_WindowUI.Repaint();
	}

	WindowUIManager::WindowUIManager(Window& window)
		:
		m_Window(window),
		m_Renderer(renderer)
	{}

	WindowUIManager::WindowUIManager(Window& window, Renderer& renderer, Compositor& compositor)
		:
		m_Window(window),
		m_Renderer(renderer),
		m_Compositor(&compositor)
	{
		m_Repainter.emplace(*this);
		m_Window.AddEventHandler(*m_Repainter, Events::EventType::WINDOW_REPAINT);
		m_Window.AddEventHandler(*m_Repainter, Events::EventType::WINDOW_RESIZE);
	}

	WindowUIManager::~WindowUIManager()
	{
		if (m_Compositor)
			m_Window.RemoveEventHandler(*m_Repainter);
	}

	void WindowUIManager::SetAutoRepaint(bool autoRepaint)
	{
		m_AutoRepaint = autoRepaint;
	}

	void WindowUIManager::SetRepaintOnResize(bool repaintOnResize)
	{
		m_RepaintOnResize = repaintOnResize;
	}

	void WindowUIManager::AddElement(Elements::Element& element)
	{
		if (element.m_IsInWindowUI)
			return;

		m_Elements.push_back(&element);

		element.OnAdd();
		element.m_IsInWindowUI = true;

		Repaint(true);
	}

	bool WindowUIManager::RemoveElement(Elements::Element& element)
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

	void WindowUIManager::ClearElements()
	{
		for (uint64_t i = 0; i < m_Elements.size(); i++)
			m_Elements[i]->m_IsInWindowUI = false;

		m_Elements.clear();
	}

	void WindowUIManager::SetBackgroundColor(Math::Vector4f backgroundColor)
	{
		if (!m_Compositor)
			return;

		m_Compositor->SetClearColor(backgroundColor);
	}

	Window& WindowUIManager::GetWindow()
	{
		return m_Window;
	}

	void WindowUIManager::Repaint(bool isAutomatic)
	{
		if (!m_Compositor || (isAutomatic && !m_AutoRepaint))
			return;
		
		m_Compositor->RenderFrame();
	}

	ScopedNoRepaint::ScopedNoRepaint(WindowUIManager& windowUI)
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
