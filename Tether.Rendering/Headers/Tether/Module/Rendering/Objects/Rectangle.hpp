#pragma once

#include <Tether/Common/Color.hpp>

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
		void SetColor(Color color);
		float GetWidth();
		float GetHeight();
		Color GetColor();
	protected:
		Rectangle(ObjectRenderer* pObjectRenderer);
		
		float width = 0.0f;
		float height = 0.0f;

		Color color;
	};
}