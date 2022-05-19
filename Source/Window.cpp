#include <Tether/Window.hpp>
#include <Tether/Controls/Control.hpp>

using namespace Tether;

void Tether::Window::AddElement(Elements::Element* pElement, bool repaint)
{
	if (!initialized)
	{
		DispatchNoInit("Window::AddElement");
		return;
	}

	elements.push_back(pElement);

	if (repaint)
		Repaint();
}

bool Tether::Window::RemoveElement(Elements::Element* pElement, bool repaint)
{
	if (!initialized)
	{
		DispatchNoInit("Window::RemoveElement");
		return false;
	}

	for (uint64_t i = 0; i < elements.size(); i++)
		if (elements[i] == pElement)
		{
			elements.erase(elements.begin() + i);

			if (repaint)
				Repaint();

			return true;
		}

	return false;
}

void Tether::Window::SetBackgroundColor(Color backgroundColor)
{
	if (!initialized)
	{
		DispatchNoInit("Window::SetBackgroundColor");
		return;
	}

	this->backgroundColor = backgroundColor;
}

void Tether::Window::ClearElements()
{
	if (!initialized)
	{
		DispatchNoInit("Window::ClearElements");
		return;
	}
}

void Tether::Window::Repaint()
{
	if (!initialized)
	{
		DispatchNoInit("Window::Repaint");
		return;
	}

	SpawnEvent(Events::EventType::WINDOW_REPAINT, 
	[this](Events::EventHandler* pEventHandler)
	{
		pEventHandler->OnWindowRepaint(Events::WindowRepaintEvent());
	});
	
	// TODO: Element rendering
}

bool Tether::Window::InitGraphics()
{
	if (!initialized)
	{
		DispatchNoInit("Window::InitGraphics");
		return false;
	}

	return true;
}

void Tether::Window::DisposeGraphics()
{
	if (!initialized)
	{
		DispatchNoInit("Window::DisposeGraphics");
		return;
	}
}
