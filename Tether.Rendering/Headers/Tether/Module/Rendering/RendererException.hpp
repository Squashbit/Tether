#pragma once

#include <exception>

namespace Tether::Rendering
{
	class RendererException : public std::exception
	{
	public:
		RendererException() = default;
		RendererException(const char* message)
			:
			std::exception(message)
		{}
	};
}