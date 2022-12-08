#include <Tether/Common/IDisposable.hpp>

using namespace Tether;

bool IDisposable::IsInitialized() const
{
    return initialized;
}

void IDisposable::Dispose()
{
    if (!initialized)
        return;
    initialized = false;
    
    OnDispose();
}