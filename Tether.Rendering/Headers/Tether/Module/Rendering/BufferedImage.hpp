#pragma once

#include <Tether/Common/Defs.hpp>

#include <cstdint>

namespace Tether::Rendering
{
	struct BufferedImageInfo
	{
		uint32_t width;
		uint32_t height;
		uint32_t channels;
		uint32_t* pixelData;
	};

	class TETHER_EXPORT BufferedImage
	{
	public:
		virtual ~BufferedImage() = default;
		TETHER_NO_COPY(BufferedImage);
	protected:
		BufferedImage() = default;
	};
}