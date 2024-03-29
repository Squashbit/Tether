#pragma once

#include <Tether/Rendering/Resources/Resource.hpp>

#include <cstdint>

namespace Tether::Rendering::Resources
{
	struct BufferedImageInfo
	{
		uint32_t width;
		uint32_t height;
		uint32_t channels;
		uint32_t* pixelData;
	};

	class TETHER_EXPORT BufferedImage : public Resource
	{
	public:
		virtual ~BufferedImage() = default;
		TETHER_NO_COPY(BufferedImage);

		uint32_t GetWidth();
		uint32_t GetHeight();
	protected:
		BufferedImage(const BufferedImageInfo& info);
	private:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
	};
}