#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/Elements/Element.hpp>
#include <Tether/Module/Rendering/WindowUI.hpp>

#include <vector>
#include <functional>

namespace Tether::Rendering::Elements
{
	class TETHER_EXPORT BorderedElement
	{
	public:
		BorderedElement(WindowUI& windowUI, Element& element);

		void SetBorderSize(float borderSize);
		void SetBorderColor(Math::Vector4f color);

		const float GetBorderSize() const;
	protected:
		void UpdateBorderTransform();
		void UpdateBorderStyle();

		void AddBorderObjects(std::vector<std::reference_wrapper<Objects::Object>>& objects);
	private:
		float m_BorderSize = 0.0f;

		Scope<Objects::Rectangle> m_BackgroundRect;
		Scope<Objects::Rectangle> m_BorderRect;

		WindowUI& m_BorderWindowUI;
		Element& m_Element;
	};
}