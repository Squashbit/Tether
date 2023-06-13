#pragma once

#include <Tether/Math/Types.hpp>
#include <Tether/Rendering/Resources/BufferedImage.hpp>
#include <Tether/Rendering/Resources/Font.hpp>

#include <string>

namespace Tether::Rendering
{
	class Renderer
	{
	public:
		enum class TextJustify
		{
			LEFT,
			CENTER,
			RIGHT
		};

		virtual void StartRender() = 0;
		virtual void EndRender() = 0;

		virtual void FillRect(float x, float y, float width, float height,
			Math::Vector4f color) = 0;
		virtual void DrawImage(float x, float y, float width, float height,
			Resources::BufferedImage& image) = 0;

		virtual void DrawText(
			float x, float y,
			std::string_view text, Resources::Font& font,
			Math::Vector4f color = Math::Vector4f(1.0f),
			float scale = 1.0f, TextJustify justify = TextJustify::RIGHT
		) = 0;
	};
}