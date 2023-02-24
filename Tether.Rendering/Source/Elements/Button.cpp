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
		bool withinWidth = info.GetRelativeY() < m_Button.m_X + m_Button.m_Width 
			+ m_Button.m_BorderSize;
		bool withinHeight = info.GetRelativeY() < m_Button.m_Y + m_Button.m_Height 
			+ m_Button.m_BorderSize;;
		
		if (!withinX || !withinY || !withinWidth || !withinHeight)
			return;

		m_Button.Click();
	}

	Button::Button(WindowUI& windowUI)
		:
		Element(windowUI),
		m_BackgroundRect(m_Renderer.CreateObject<Objects::Rectangle>()),
		m_BorderRect(m_Renderer.CreateObject<Objects::Rectangle>())
	{
		m_Objects.push_back(*m_BorderRect);
		m_Objects.push_back(*m_BackgroundRect);
	}

	void Button::SetBorderSize(float borderSize)
	{
		m_BorderSize = borderSize;
	}

	void Button::Click()
	{
		std::cout << "Button clicked\n";
	}

	void Button::UpdateTransform()
	{
		m_BackgroundRect->SetX(m_X);
		m_BackgroundRect->SetY(m_Y);
		m_BackgroundRect->SetWidth(m_Width);
		m_BackgroundRect->SetHeight(m_Height);

		m_BorderRect->SetX(m_X - m_BorderSize);
		m_BorderRect->SetY(m_Y - m_BorderSize);
		m_BorderRect->SetWidth(m_Width + m_BorderSize * 2.0f);
		m_BorderRect->SetHeight(m_Height + m_BorderSize * 2.0f);
	}

	void Button::UpdateStyle()
	{
		m_BackgroundRect->SetColor(m_BackgroundColor);
		m_BorderRect->SetColor(m_Color);
	}
}