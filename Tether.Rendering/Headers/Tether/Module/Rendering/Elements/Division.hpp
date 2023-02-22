#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/Elements/Element.hpp>

namespace Tether::Rendering::Elements
{
	class TETHER_EXPORT Division : public Element
	{
	public:
		Division(WindowUI& windowUI);
	private:
		void UpdateStyle();


	};
}
