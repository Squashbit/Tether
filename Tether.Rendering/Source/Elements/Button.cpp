#include <Tether/Rendering/Elements/Button.hpp>

namespace Tether::Rendering::Elements
{
	Button::ClickListener::ClickListener(Button& button)
		:
		m_Button(button)
	{}

	void Button::ClickListener::OnMouseClick(Input::MouseClickInfo& info)
	{
		float borderSize = m_Button.GetBorderSize();
		int clickX = info.GetRelativeX();
		int clickY = info.GetRelativeY();

		bool withinX = clickX >= m_Button.m_X - borderSize;
		bool withinY = clickY >= m_Button.m_Y - borderSize;
		bool withinWidth = clickX < m_Button.m_X + m_Button.m_Width + borderSize;
		bool withinHeight = clickY < m_Button.m_Y + m_Button.m_Height + borderSize;

		if (!withinX || !withinY || !withinWidth || !withinHeight ||
			info.IsPressed())
			return;

		m_Button.Click();
	}

	Button::Button(WindowUIManager& windowUI)
		:
		Element(windowUI),
		BorderedElement(windowUI, *this),
		m_Listener(*this)
	{
		m_Window.AddInputListener(m_Listener, Input::InputType::MOUSE_CLICK);
	}

	Button::~Button()
	{
		m_Window.RemoveInputListener(m_Listener);
	}

	void Button::SetText(std::string_view text)
	{
		m_Text = text;
		Repaint();
	}

	void Button::SetFont(Resources::Font& font)
	{
		m_pFont = &font;
		Repaint();
	}

	void Button::SetOnClickFunction(std::function<void()> clickFunction)
	{
		m_ClickFunction = clickFunction;
	}

	void Button::OnRender(Renderer& renderer)
	{
		DrawBorder(renderer);

		if (m_pFont)
			renderer.DrawText(m_X + m_Width / 2.0f, m_Y + m_Height / 2.0f, m_Text,
				*m_pFont, m_Color, 1.0f, Renderer::TextJustify::CENTER);
	}

	void Button::Click()
	{
		if (!m_ClickFunction)
			return;

		m_ClickFunction();
		Repaint();
	}
}