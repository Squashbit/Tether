#include <Tether/Devices/DeviceManager.hpp>

using namespace Tether::Devices;

DeviceManager::DeviceManager(SimpleWindow* pWindow)
    :
    pWindow(pWindow),
    app(&Application::Get())
{
    
}
