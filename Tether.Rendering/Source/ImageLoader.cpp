#include <Tether/Module/Rendering/ImageLoader.hpp>
#include <stb_image.h>

namespace Tether::Rendering
{
	ImageLoader::ImageLoader(const std::string& path)
		:
		m_Path(path)
	{}

	ImageLoader::~ImageLoader()
	{
		FreeImage();
	}

	bool ImageLoader::Load()
	{
		FreeImage();

		int width, height, channels;

		imageInfo.pixelData = (uint32_t*)stbi_load(
			m_Path.c_str(), 
			&width,
			&height,
			&channels,
			STBI_rgb_alpha
		);

		if (!imageInfo.pixelData)
			return false;

		imageInfo.width = width;
		imageInfo.height = height;
		imageInfo.channels = channels;

		loaded = true;
		return true;
	}

	Resources::BufferedImageInfo& ImageLoader::GetImageInfo()
	{
		return imageInfo;
	}

	void ImageLoader::FreeImage()
	{
		if (!loaded)
			return;

		stbi_image_free(imageInfo.pixelData);

		loaded = false;
	}
}