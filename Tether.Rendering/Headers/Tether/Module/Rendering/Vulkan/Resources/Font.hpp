#pragma once

#include <Tether/Module/Rendering/Resources/Font.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT Font : public Resources::Font
	{
	public:
		Font(const std::string& fontPath);
		~Font();
		TETHER_NO_COPY(Font);
	private:

	};
}