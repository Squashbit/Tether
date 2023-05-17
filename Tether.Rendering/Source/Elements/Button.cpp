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

	Button::Button(WindowUI& windowUI)
		:
		Element(windowUI),
		BorderedElement(windowUI, *this),
		m_TextObject(m_Renderer.CreateObject<Objects::Text>()),
		m_Listener(*this)
	{
		AddBorderObjects(m_Objects);

		m_Window.AddInputListener(m_Listener, Input::InputType::MOUSE_CLICK);

		m_TextObject->SetJustify(Objects::Text::Justify::CENTER);
		m_Objects.push_back(*m_TextObject);
	}

	Button::~Button()
	{
		m_Window.RemoveInputListener(m_Listener);
	}

	void Button::SetText(std::string_view text)
	{
		m_TextObject->SetText(text.data());
		m_WindowUI.Repaint(true);
	}

	void Button::SetFont(Resources::Font& font)
	{
		m_TextObject->SetFont(font);
		m_WindowUI.Repaint(true);
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
		UpdateBorderTransform();

		m_TextObject->SetX(m_X + m_Width / 2.0f);
		m_TextObject->SetY(m_Y + m_Height / 2.0f);
	}

	void Button::UpdateStyle()
	{
		UpdateBorderStyle();
		m_TextObject->SetColor(m_Color);
	}
}