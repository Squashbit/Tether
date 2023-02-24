#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/WindowUI.hpp>

#include <Tether/Module/Rendering/Objects/Rectangle.hpp>
#include <Tether/Module/Rendering/Objects/Text.hpp>
#include <Tether/Module/Rendering/Elements/Element.hpp>

namespace Tether::Rendering::Elements
{
	class TETHER_EXPORT Button : public Element
	{
	public:
		Button(WindowUI& windowUI);
		~Button();

		void SetBorderSize(float borderSize);
		void SetBorderColor(const Math::Vector4f& color);

		void SetText(std::string_view text);
		void SetFont(Resources::Font& font);

		void SetOnClickFunction(std::function<void()> clickFunction);
	private:
		class ClickListener : public Input::InputListener
		{
		public:
			ClickListener(Button& button);

			void OnMouseClick(Input::MouseClickInfo& info) override;
		private:
			Button& m_Button;
		};

		void UpdateTransform();
		void UpdateStyle();
		
		float m_BorderSize = 1.0f;
		Math::Vector4f m_BorderColor;

		Scope<Objects::Rectangle> m_BackgroundRect;
		Scope<Objects::Rectangle> m_BorderRect;
		Scope<Objects::Text> m_TextObject;

		std::function<void()> m_ClickFunction;

		std::string m_Text;

		ClickListener m_Listener;
	};
}
