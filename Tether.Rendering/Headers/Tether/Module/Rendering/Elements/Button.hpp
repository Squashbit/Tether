#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/WindowUI.hpp>

#include <Tether/Module/Rendering/Objects/Rectangle.hpp>
#include <Tether/Module/Rendering/Elements/Element.hpp>

namespace Tether::Rendering::Elements
{
	class TETHER_EXPORT Button : public Element
	{
	public:
		Button(WindowUI& windowUI);

		void SetBorderSize(float borderSize);
	private:
		class ClickListener : public Input::InputListener
		{
		public:
			ClickListener(Button& button);

			void OnMouseClick(Input::MouseClickInfo& info) override;
		private:
			Button& m_Button;
		};

		void Click();

		void UpdateTransform();
		void UpdateStyle();

		float m_BorderSize = 1.0f;

		Scope<Objects::Rectangle> m_BackgroundRect;
		Scope<Objects::Rectangle> m_BorderRect;
	};
}
