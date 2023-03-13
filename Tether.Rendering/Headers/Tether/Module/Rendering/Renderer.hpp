#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/Objects/Image.hpp>
#include <Tether/Module/Rendering/Objects/Rectangle.hpp>
#include <Tether/Module/Rendering/Objects/Text.hpp>

#include <Tether/Module/Rendering/Resources/BufferedImage.hpp>
#include <Tether/Module/Rendering/Resources/Font.hpp>

#include <Tether/Common/TypeTools.hpp>
#include <Tether/Common/HashedType.hpp>
#include <Tether/Common/Types.hpp>

#include <Tether/Math/Types.hpp>

#include <type_traits>
#include <vector>

namespace Tether::Rendering
{
	class TETHER_EXPORT Renderer
	{
	public:
		Renderer() = default;
		virtual ~Renderer();
		TETHER_NO_COPY(Renderer);

		void AddObject(Objects::Object& object);
		bool RemoveObject(Objects::Object& object);
		bool HasObject(Objects::Object& object);
		void ClearObjects();

		const std::vector<Objects::Object*>& GetObjects() const;
	protected:
		std::vector<Objects::Object*> objects;
	};
}
