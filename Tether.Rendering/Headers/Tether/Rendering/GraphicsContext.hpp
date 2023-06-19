#pragma once

#include <Tether/Common/Ref.hpp>

#include <Tether/Window.hpp>
#include <Tether/Rendering/Resources/BufferedImage.hpp>
#include <Tether/Rendering/Resources/Font.hpp>
#include <Tether/Rendering/RenderTarget.hpp>

#include <vector>

namespace Tether::Rendering
{
	class GraphicsContext
	{
	public:
		virtual Scope<RenderTarget> CreateWindowRenderTarget(Window& window) = 0;

		virtual Scope<Resources::BufferedImage> CreateBufferedImage(
			const Resources::BufferedImageInfo& info) = 0;
		virtual Scope<Resources::Font> CreateFont(
			const std::string& fontPath) = 0; 
	};
}