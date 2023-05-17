#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Math/Vector4.hpp>

#include <Tether/Rendering/Resources/Font.hpp>
#include <Tether/Rendering/Resources/BufferedImage.hpp>

namespace Tether::Rendering
{
	class TETHER_EXPORT RenderAction
	{
	public:
		enum class TextJustify
		{
			LEFT,
			CENTER,
			RIGHT
		};

		RenderAction() = default;
		virtual ~RenderAction() = 0;

		virtual void StartRenderPass() = 0;
		virtual void EndRenderPass() = 0;

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
