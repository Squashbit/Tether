#include <Tether/Module/Rendering/RenderingModule.hpp>
#include <iostream>

using namespace Tether::Rendering;

RenderingModule RenderingModule::internal = RenderingModule();

bool RenderingModule::OnInit()
{
	return true;
}

void RenderingModule::OnDispose()
{
	DisposeVulkan();
}

RenderingModule& RenderingModule::Get()
{
	return internal;
}