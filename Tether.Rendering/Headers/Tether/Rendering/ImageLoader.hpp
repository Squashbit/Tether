#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Rendering/Resources/BufferedImage.hpp>

#include <string>
#include <vector>

namespace Tether::Rendering
{
	class TETHER_EXPORT ImageLoader
	{
	public:
		ImageLoader(const std::string& path);
		~ImageLoader();
		TETHER_NO_COPY(ImageLoader);

		bool Load();

		Resources::BufferedImageInfo& GetImageInfo();
	private:
		void FreeImage();

		std::string m_Path = "";

		bool loaded = false;
		Resources::BufferedImageInfo imageInfo;
	};
}