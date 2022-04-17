#include <Tether/Events/EventHandler.hpp>
#include <Tether/Common/VectorUtils.hpp>
#include <Tether/IWindow.hpp>

using namespace Tether::Events;

EventHandler::~EventHandler()
{
	std::vector<IWindow*> pWindows(windows);
	for (uint64_t i = 0; i < pWindows.size(); i++)
		pWindows[i]->RemoveEventHandler(this);
}

void EventHandler::OnAdd(IWindow* pWindow)
{
	if (VectorUtils::Contains(windows, pWindow))
		return;

	windows.push_back(pWindow);
}

void EventHandler::OnRemove(IWindow* pWindow)
{
	if (!VectorUtils::Contains(windows, pWindow))
		return;
	
	windows.erase(std::find(windows.begin(), windows.end(), pWindow));
}