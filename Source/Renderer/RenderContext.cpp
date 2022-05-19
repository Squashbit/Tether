#include <Tether/Renderer/RenderContext.hpp>

using namespace Tether::Renderer;

RenderContext::RenderContext()
{
	initialized = true;
}

bool RenderContext::Init(IRenderContextNative* pNative)
{
	native = pNative;
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
	native = nullptr; // bruh


}
