#pragma once

#include <Tether/Module/Rendering/Objects/Object.hpp>

#include <string>

namespace Tether::Rendering::Objects
{
	class TETHER_EXPORT Text : public Object
	{
	public:
		virtual ~Text() = default;
		TETHER_NO_COPY(Text);

		virtual void SetText(const std::string& text) = 0;
	protected:
		Text(ObjectRenderer* pObjectRenderer);
	};
}