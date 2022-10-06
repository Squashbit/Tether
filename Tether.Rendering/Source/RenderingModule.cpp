#include <Tether/Module/Rendering/RenderingModule.hpp>
#include <iostream>

using namespace Tether::Rendering;

RenderingModule RenderingModule::internal = RenderingModule();

bool RenderingModule::OnInit()
{
	std::cout << "Rendering module initialized!" << std::endl;
	return true;
}

RenderingModule& RenderingModule::Get()
{
	return internal;
}