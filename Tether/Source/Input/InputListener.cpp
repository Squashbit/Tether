#include <Tether/Input/InputListener.hpp>
#include <Tether/Common/VectorUtils.hpp>
#include <Tether/SimpleWindow.hpp>

#include <algorithm>

using namespace Tether::Input;

InputListener::~InputListener()
{
	std::vector<SimpleWindow*> pWindows(windows);
	for (uint64_t i = 0; i < pWindows.size(); i++)
		pWindows[i]->RemoveInputListener(this);
}

void InputListener::OnAdd(SimpleWindow* pWindow)
{
	if (VectorUtils::Contains(windows, pWindow))
		return;

	windows.push_back(pWindow);
}

void InputListener::OnRemove(SimpleWindow* pWindow)
{
	if (!VectorUtils::Contains(windows, pWindow))
		return;

	windows.erase(std::find(windows.begin(), windows.end(), pWindow));
}