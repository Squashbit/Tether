#pragma once

#include <Tether/Module/Rendering/Common/Defs.hpp>

#include <Tether/Module/Rendering/Objects/Object.hpp>
#include <Tether/Module/Rendering/Resources/Resource.hpp>

#include <Tether/Common/TypeTools.hpp>
#include <Tether/Common/HashedType.hpp>
#include <Tether/Common/Types.hpp>

#include <Tether/Math/Types.hpp>

#include <type_traits>
#include <typeinfo>
#include <vector>
#include <unordered_map>

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