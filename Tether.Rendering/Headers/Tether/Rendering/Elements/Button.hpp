#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Rendering/Elements/Element.hpp>
#include <Tether/Rendering/Elements/Common/BorderedElement.hpp>

namespace Tether::Rendering::Elements
{
	class TETHER_EXPORT Button : public Element, public BorderedElement
	{
	public:
		Button(WindowUIManager& windowUI);
		~Button();

		void SetText(std::string_view text);
		void SetFont(Resources::Font& font);

		void SetOnClickFunction(std::function<void()> clickFunction);
	private:
		void OnRender(Renderer& renderer) override;

		class ClickListener : public Input::InputListener
		{
		public:
			ClickListener(Button& button);

			void OnMouseClick(Input::MouseClickInfo& info) override;
		private:
			Button& m_Button;
		};

		void Click();

		ClickListener m_Listener;
		std::function<void()> m_ClickFunction;

		std::string m_Text;
		Resources::Font* m_pFont = nullptr;
	};
}
