#include <Tether/Module/Rendering/Elements/Button.hpp>
#include <iostream>

namespace Tether::Rendering::Elements
{
	Button::ClickListener::ClickListener(Button& button)
		:
		m_Button(button)
	{}

	void Button::ClickListener::OnMouseClick(Input::MouseClickInfo& info)
	{
		bool withinX = info.GetRelativeX() >= m_Button.m_X - m_Button.m_BorderSize;
		bool withinY = info.GetRelativeY() >= m_Button.m_Y - m_Button.m_BorderSize;
		bool withinWidth = info.GetRelativeX() < m_Button.m_X + m_Button.m_Width
			+ m_Button.m_BorderSize;
		bool withinHeight = info.GetRelativeY() < m_Button.m_Y + m_Button.m_Height
			+ m_Button.m_BorderSize;

		if (!withinX || !withinY || !withinWidth || !withinHeight ||
			info.IsPressed())
			return;

		m_Button.Click();
	}

	Button::Button(WindowUI& windowUI)
		:
		Element(windowUI),
		m_BackgroundRect(m_Renderer.CreateObject<Objects::Rectangle>()),
		m_BorderRect(m_Renderer.CreateObject<Objects::Rectangle>()),
		m_TextObject(m_Renderer.CreateObject<Objects::Text>()),
		m_Listener(*this)
	{
		m_Window.AddInputListener(m_Listener, Input::InputType::MOUSE_CLICK);

		m_Objects.push_back(*m_BorderRect);
		m_Objects.push_back(*m_BackgroundRect);
		m_Objects.push_back(*m_TextObject);
	}

	Button::~Button()
	{
		m_Window.RemoveInputListener(m_Listener);
	}

	void Button::SetText(std::string_view text)
	{
		m_Text = text;
		m_TextObject->SetText(m_Text);

		UpdateTransform();
	}

	void Button::SetFont(Resources::Font& font)
	{
		m_TextObject->SetFont(&font);
	}

	void Button::SetOnClickFunction(std::function<void()> clickFunction)
	{
		m_ClickFunction = clickFunction;
	}

	void Button::Click()
	{
		if (!m_ClickFunction)
			return;

		m_ClickFunction();
		m_WindowUI.Repaint(true);
	}

	void Button::UpdateTransform()
	{
		m_BorderRect->SetX(m_X - m_BorderSize);
		m_BorderRect->SetY(m_Y - m_BorderSize);
		m_BorderRect->SetWidth(m_Width + m_BorderSize * 2.0f);
		m_BorderRect->SetHeight(m_Height + m_BorderSize * 2.0f);

		m_BackgroundRect->SetX(m_X);
		m_BackgroundRect->SetY(m_Y);
		m_BackgroundRect->SetWidth(m_Width);
		m_BackgroundRect->SetHeight(m_Height);

		m_TextObject->SetX(m_X);
		m_TextObject->SetY(m_Y);
	}

	void Button::UpdateStyle()
	{
		m_BorderRect->SetColor(m_BorderColor);
		m_BackgroundRect->SetColor(m_BackgroundColor);

		m_TextObject->SetColor(m_Color);
	}
}