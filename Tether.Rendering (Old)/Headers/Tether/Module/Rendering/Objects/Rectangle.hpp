#pragma once

#include <Tether/Math/Types.hpp>

#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Objects/Object.hpp>

namespace Tether::Rendering::Objects
{
	class TETHER_EXPORT Rectangle : public Object
	{
	public:
		virtual ~Rectangle() = default;
		
		void SetWidth(float value);
		void SetHeight(float value);
		void SetColor(Math::Vector4f color);
		float GetWidth() const;
		float GetHeight() const;
		Math::Vector4f GetColor();
	protected:
		Rectangle(ObjectRenderer* pObjectRenderer);
		
		float width = 0.0f;
		float height = 0.0f;

		Math::Vector4f color;
	};
}