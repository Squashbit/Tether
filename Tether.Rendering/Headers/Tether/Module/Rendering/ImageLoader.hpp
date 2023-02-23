#pragma once

#include <Tether/Common/Defs.hpp>

#include <string>
#include <vector>

namespace Tether::Rendering
{
	class TETHER_EXPORT ImageLoader
	{
	public:
		struct ImageInfo
		{
			uint32_t width;
			uint32_t height;
			uint32_t channels;
			uint32_t* pixelData;
		};

		ImageLoader(const std::string& path);
		~ImageLoader();
		TETHER_NO_COPY(ImageLoader);

		bool Load();

		const ImageInfo& GetImageInfo() const;
	private:
		void FreeImage();

		std::string m_Path = "";

		bool loaded = false;
		ImageInfo imageInfo;
	};
}