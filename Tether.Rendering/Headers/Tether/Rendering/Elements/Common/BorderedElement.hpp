#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Rendering/Elements/Element.hpp>
#include <Tether/Rendering/WindowUI.hpp>

#include <vector>
#include <functional>

namespace Tether::Rendering::Elements
{
	class TETHER_EXPORT BorderedElement
	{
	public:
		BorderedElement(WindowUIManager& windowUI, Element& element);

		void SetBorderSize(float borderSize);
		void SetBorderColor(Math::Vector4f color);

		const float GetBorderSize() const;
	private:
		float m_BorderSize = 0.0f;

		WindowUIManager& m_BorderWindowUI;
		Element& m_Element;
	};
}