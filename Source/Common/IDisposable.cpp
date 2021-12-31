#include <Tether/Common/IDisposable.hpp>

using namespace Tether;

IDisposable::~IDisposable()
{
    Dispose();
}

void IDisposable::Dispose()
{
    if (!initialized)
        return;
    initialized = false;
    
    OnDispose();
}