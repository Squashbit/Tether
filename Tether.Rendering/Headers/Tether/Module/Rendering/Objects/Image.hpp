#pragma once

#include <Tether/Common/Color.hpp>

#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Objects/Object.hpp>

namespace Tether::Rendering::Objects
{
	class TETHER_EXPORT Image : public Object
	{
	public:
		virtual ~Image() = default;
		
		void SetX(float value);
		void SetY(float value);
		void SetWidth(float value);
		void SetHeight(float value);
		float GetX();
		float GetY();
		float GetWidth();
		float GetHeight();

		inline static HashedString typeName =
			HashedString(TypeTools::GetTypeName<Image>());
	protected:
		Image(UIRenderer* pRenderer);
		
		float x = 0.0f;
		float y = 0.0f;
		float width = 0.0f;
		float height = 0.0f;
	};
}