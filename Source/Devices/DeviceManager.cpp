#include <Tether/Devices/DeviceManager.hpp>

using namespace Tether::Devices;

DeviceManager::~DeviceManager()
{

}

DeviceManager::DeviceManager()
{

}

bool DeviceManager::Init(IWindow* pWindow)
{
    this->pWindow = pWindow;
    return true;
}