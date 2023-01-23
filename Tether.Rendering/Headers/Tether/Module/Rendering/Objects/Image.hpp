#pragma once

#include <Tether/Common/Color.hpp>

#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Objects/Object.hpp>

#include <Tether/Module/Rendering/BufferedImage.hpp>

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

		virtual void SetImage(BufferedImage* image) = 0;
	protected:
		Image(Renderer* pRenderer);
		
		float x = 0.0f;
		float y = 0.0f;
		float width = 0.0f;
		float height = 0.0f;
	};
}