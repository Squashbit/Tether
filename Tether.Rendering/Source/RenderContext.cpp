#include <Tether/Module/Rendering/RenderContext.hpp>

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

bool RenderContext::RenderFrame()
{
	if (native)
		return native->RenderFrame();

	return false;
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
