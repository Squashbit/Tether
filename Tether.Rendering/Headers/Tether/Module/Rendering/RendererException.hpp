#pragma once

#include <stdexcept>

namespace Tether::Rendering
{
	class RendererException : public std::runtime_error
	{
	public:
		RendererException() = default;
		RendererException(const char* message)
			:
			std::runtime_error(message)
		{}
	};
}