#ifndef _TETHER_RENDERING_RENDERINGMODULE_HPP
#define _TETHER_RENDERING_RENDERINGMODULE_HPP

#include <Tether/Module.hpp>
#include <Tether/Module/Rendering/Common/Defs.hpp>

namespace Tether::Rendering
{
	class TETHER_EXPORT RenderingModule : public Module
	{
	public:
		TETHER_NO_COPY(RenderingModule);
		RenderingModule() : Module(this, "Tether.Rendering") {}

		static RenderingModule& Get();
	private:
		bool OnInit();

		static RenderingModule internal;
	};
}

#endif //_TETHER_RENDERING_RENDERINGMODULE_HPP