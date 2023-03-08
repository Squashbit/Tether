#pragma once

#include <Tether/Module/Rendering/RendererException.hpp>

namespace Tether::Rendering
{
	class IncompatibleDriverException : public RendererException
	{
	public:
		IncompatibleDriverException() = default;
		IncompatibleDriverException(const char* message)
			:
			RendererException(message)
		{}
	};
}