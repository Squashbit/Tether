#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/WindowUI.hpp>

#include <Tether/Module/Rendering/Objects/Rectangle.hpp>
#include <Tether/Module/Rendering/Elements/Element.hpp>
#include <Tether/Module/Rendering/Elements/Common/BorderedElement.hpp>

namespace Tether::Rendering::Elements
{
	class TETHER_EXPORT Division : public Element, BorderedElement
	{
	public:
		Division(WindowUI& windowUI);
	private:
		void UpdateTransform() override;
		void UpdateStyle() override;
	};
}
