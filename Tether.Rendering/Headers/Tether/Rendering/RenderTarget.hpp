#pragma once

#include <Tether/Math/Types.hpp>
#include <Tether/Rendering/Resources/BufferedImage.hpp>
#include <Tether/Rendering/Resources/Font.hpp>

#include <Tether/Rendering/Renderer.hpp>

#include <string>

namespace Tether::Rendering
{
	class RenderTarget
	{
	public:
		virtual void StartRender() = 0;
		virtual bool EndRender() = 0;

		virtual Renderer& GetRenderer() = 0;
	};
}