#include <Tether/Module.hpp>
#include <Tether/Application.hpp>

using namespace Tether;

Module::Module(Module* pModule, const char* moduleName)
{
	this->moduleName = moduleName;

	Application::Get().RegisterModule(pModule);
}

const char* Module::GetModuleName()
{
	return this->moduleName;
}