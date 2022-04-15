#include <Tether/Common/IDisposable.hpp>

using namespace Tether;

IDisposable::~IDisposable()
{
    Dispose();
}

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