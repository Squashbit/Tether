#pragma once

#include <Tether/Common/Ref.hpp>

#include <Tether/Module/Rendering/Objects/Image.hpp>
#include <Tether/Module/Rendering/Objects/Rectangle.hpp>
#include <Tether/Module/Rendering/Objects/Text.hpp>

#include <Tether/Module/Rendering/Resources/BufferedImage.hpp>
#include <Tether/Module/Rendering/Resources/Font.hpp>

#include <vector>

namespace Tether::Rendering
{
	class GraphicsContext
	{
	public:
		virtual Scope<Objects::Rectangle> CreateRectangleObject() = 0;
		virtual Scope<Objects::Image> CreateImageObject() = 0;
		virtual Scope<Objects::Text> CreateTextObject() = 0;

		virtual Scope<Resources::BufferedImage> CreateBufferedImage(
			const Resources::BufferedImageInfo& info) = 0;
		virtual Scope<Resources::Font> CreateFont(
			const std::string& fontPath) = 0;
	};
}