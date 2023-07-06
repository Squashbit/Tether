#pragma once

#include <Tether/Math/Types.hpp>
#include <Tether/Rendering/Resources/BufferedImage.hpp>
#include <Tether/Rendering/Resources/Font.hpp>

#include <Tether/Rendering/Renderer.hpp>

#include <string>

namespace Tether::Rendering
{
	class TETHER_EXPORT RenderTarget
	{
	public:
		virtual ~RenderTarget() = default;

		virtual void StartRender(Math::Vector4f clearColor = 
			Math::Vector4f(0.0f, 0.0f, 0.0f, 1.0f)) = 0;
		virtual bool EndRender() = 0;

		virtual Renderer& GetRenderer() = 0;
	};
}