#include <Tether/Renderer/RenderContext.hpp>

using namespace Tether::Renderer;

RenderContext::RenderContext()
{}

bool RenderContext::Init(IRenderContextNative* pNative)
{
	if (initialized)
		return false;

	native = pNative;

	initialized = true;
	return true;
}

IRenderContextNative* const RenderContext::GetNative() const
{
	return native;
}

void RenderContext::OnDispose()
{
	if (native)
		native->Dispose();
}
