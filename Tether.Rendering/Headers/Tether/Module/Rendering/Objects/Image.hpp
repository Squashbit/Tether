#pragma once

#include <Tether/Module/Rendering/Objects/Object.hpp>

#include <Tether/Module/Rendering/Resources/BufferedImage.hpp>

namespace Tether::Rendering::Objects
{
	class TETHER_EXPORT Image : public Object
	{
	public:
		virtual ~Image() = default;
		
		void SetWidth(float value);
		void SetHeight(float value);
		float GetWidth();
		float GetHeight();

		virtual void SetImage(Resources::BufferedImage& image) = 0;
	protected:
		Image(ObjectRenderer& objectRenderer);

		float width = 0.0f;
		float height = 0.0f;
	};
}