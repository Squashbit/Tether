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
		
		void SetX(float value);
		void SetY(float value);
		void SetWidth(float value);
		void SetHeight(float value);
		void SetColor(Color color);
		float GetX();
		float GetY();
		float GetWidth();
		float GetHeight();
		Color GetColor();

		inline static HashedString typeName =
			HashedString(TypeTools::GetTypeName<Rectangle>());
	protected:
		Rectangle(UIRenderer* pRenderer);
		
		float x = 0.0f;
		float y = 0.0f;
		float width = 0.0f;
		float height = 0.0f;

		Color color;
	};
}