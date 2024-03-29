#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Rendering/Elements/Element.hpp>
#include <Tether/Rendering/Elements/Common/BorderedElement.hpp>

namespace Tether::Rendering::Elements
{
	class TETHER_EXPORT Division : public Element, BorderedElement
	{
	public:
		Division(WindowUIManager& windowUI);
	private:
		void OnRender(Renderer& renderer) override;
	};
}
