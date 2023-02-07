#include <Tether/Module/Rendering/GraphicalWindow.hpp>
#include <Tether/Controls/Control.hpp>

using namespace Tether;
using namespace Tether::Rendering;

GraphicalWindow::GraphicalWindow(int width, int height, const char* title, bool visible)
	:
	SimpleWindow(width, height, title, visible)
{}

void GraphicalWindow::AddElement(Elements::Element* pElement, bool repaint)
{
	elements.push_back(pElement);

	if (repaint)
		Repaint();
}

bool GraphicalWindow::RemoveElement(Elements::Element* pElement, bool repaint)
{
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

void GraphicalWindow::SetBackgroundColor(Math::Vector4f backgroundColor)
{
	this->backgroundColor = backgroundColor;
}

void GraphicalWindow::ClearElements()
{
	
}

void GraphicalWindow::Repaint()
{
	SpawnEvent(Events::EventType::WINDOW_REPAINT, 
	[this](Events::EventHandler* pEventHandler)
	{
		pEventHandler->OnWindowRepaint(Events::WindowRepaintEvent());
	});
	
	// TODO: Element rendering
}

bool GraphicalWindow::InitGraphics()
{
	return true;
}

void GraphicalWindow::DisposeGraphics()
{
	
}
