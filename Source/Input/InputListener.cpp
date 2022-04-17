#include <Tether/Input/InputListener.hpp>
#include <Tether/Common/VectorUtils.hpp>
#include <Tether/IWindow.hpp>

#include <algorithm>

using namespace Tether::Input;

InputListener::~InputListener()
{
	for (uint64_t i = 0; i < windows.size(); i++)
		windows[i]->RemoveInputListener(this);
}

void InputListener::OnAdd(IWindow* pWindow)
{
	if (VectorUtils::Contains(windows, pWindow))
		return;

	windows.push_back(pWindow);
}

void InputListener::OnRemove(IWindow* pWindow)
{
	if (!VectorUtils::Contains(windows, pWindow))
		return;

	windows.erase(std::find(windows.begin(), windows.end(), pWindow));
}