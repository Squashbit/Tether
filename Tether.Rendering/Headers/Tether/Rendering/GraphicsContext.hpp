#pragma once

#include <Tether/Common/Ref.hpp>

#include <Tether/Rendering/WindowRenderer.hpp>

#include <Tether/Rendering/Resources/BufferedImage.hpp>
#include <Tether/Rendering/Resources/Font.hpp>

#include <vector>

namespace Tether::Rendering
{
	class GraphicsContext
	{
	public:
		virtual Scope<WindowRenderer> CreateWindowRenderer(Window& window) = 0;

		virtual Scope<Resources::BufferedImage> CreateBufferedImage(
			const Resources::BufferedImageInfo& info) = 0;
		virtual Scope<Resources::Font> CreateFont(
			const std::string& fontPath) = 0;
	};
}