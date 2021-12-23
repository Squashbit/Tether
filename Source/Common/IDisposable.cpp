#include <Link/Common/IDisposable.hpp>

using namespace Link;

IDisposable::~IDisposable()
{
    Dispose();
}

void IDisposable::Dispose()
{
    if (!initialized)
        return;
    
    OnDispose();

    initialized = false;
}