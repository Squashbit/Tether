#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/Ref.hpp>

#include <Tether/Math/Vector4.hpp>
#include <Tether/Math/Types.hpp>

#include <Tether/Rendering/Renderer.hpp>
#include <Tether/Rendering/Resources/Font.hpp>
#include <Tether/Rendering/Resources/BufferedImage.hpp>

namespace Tether::Rendering
{
	class TETHER_EXPORT WindowRenderer : public Renderer
	{
	public:
		WindowRenderer() = default;
		virtual ~WindowRenderer() = 0;
	};
}