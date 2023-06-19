#include <Tether/Rendering/WindowUIManager.hpp>
#include <Tether/Rendering/Elements/Element.hpp>

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
		m_Repainter(*this)
	{
		m_Window.AddEventHandler(m_Repainter, Events::EventType::WINDOW_REPAINT);
		m_Window.AddEventHandler(m_Repainter, Events::EventType::WINDOW_RESIZE);
	}

	WindowUIManager::~WindowUIManager()
	{
		m_Window.RemoveEventHandler(m_Repainter);
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
		m_ClearColor = backgroundColor;
	}

	Window& WindowUIManager::GetWindow() const
	{
		return m_Window;
	}

	void WindowUIManager::SetRenderer(Renderer& renderer)
	{
		m_pRenderer = &renderer;
	}

	void WindowUIManager::Repaint(bool isAutomatic)
	{
		if (isAutomatic && !m_AutoRepaint || !m_pRenderer)
			return;
		
		for (Elements::Element* pElement : m_Elements)
			pElement->OnRender(*m_pRenderer);
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
