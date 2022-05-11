#include <Tether/Renderer/RenderContext.hpp>

using namespace Tether::Renderer;

RenderContext::RenderContext()
{}

void RenderContext::OnDispose()
{
	if (native)
		delete native;
}
