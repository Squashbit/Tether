#pragma once

#include <exception>

namespace Tether::Rendering::Vulkan
{
	class VulkanException : public std::exception
	{
	public:
		VulkanException() = default;
		VulkanException(const char* const message)
			:
			std::exception(message)
		{}
	};
}