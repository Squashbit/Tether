#include <Tether/Module/Rendering/GraphicalWindow.hpp>
#include <Tether/Controls/Control.hpp>

using namespace Tether;
using namespace Tether::Rendering;

void GraphicalWindow::AddElement(Elements::Element* pElement, bool repaint)
{
	if (!initialized)
	{
		DispatchNoInit("GraphicalWindow::AddElement");
		return;
	}

	elements.push_back(pElement);

	if (repaint)
		Repaint();
}

bool GraphicalWindow::RemoveElement(Elements::Element* pElement, bool repaint)
{
	if (!initialized)
	{
		DispatchNoInit("GraphicalWindow::RemoveElement");
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

void GraphicalWindow::SetBackgroundColor(Color backgroundColor)
{
	if (!initialized)
	{
		DispatchNoInit("GraphicalWindow::SetBackgroundColor");
		return;
	}

	this->backgroundColor = backgroundColor;
}

void GraphicalWindow::ClearElements()
{
	if (!initialized)
	{
		DispatchNoInit("GraphicalWindow::ClearElements");
		return;
	}
}

void GraphicalWindow::Repaint()
{
	if (!initialized)
	{
		DispatchNoInit("GraphicalWindow::Repaint");
		return;
	}

	SpawnEvent(Events::EventType::WINDOW_REPAINT, 
	[this](Events::EventHandler* pEventHandler)
	{
		pEventHandler->OnWindowRepaint(Events::WindowRepaintEvent());
	});
	
	// TODO: Element rendering
}

bool GraphicalWindow::InitGraphics()
{
	if (!initialized)
	{
		DispatchNoInit("GraphicalWindow::InitGraphics");
		return false;
	}

	return true;
}

void GraphicalWindow::DisposeGraphics()
{
	if (!initialized)
	{
		DispatchNoInit("GraphicalWindow::DisposeGraphics");
		return;
	}
}
