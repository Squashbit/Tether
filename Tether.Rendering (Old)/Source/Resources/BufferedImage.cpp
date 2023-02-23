#include <Tether/Module/Rendering/Resources/BufferedImage.hpp>

namespace Tether::Rendering::Resources
{
	BufferedImage::BufferedImage(const BufferedImageInfo& info)
		:
		m_Width(info.width),
		m_Height(info.height)
	{

	}

	uint32_t BufferedImage::GetWidth()
	{
		return m_Width;
	}

	uint32_t BufferedImage::GetHeight()
	{
		return m_Height;
	}
}