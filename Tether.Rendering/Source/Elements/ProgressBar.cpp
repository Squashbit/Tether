#include <Tether/Module/Rendering/Elements/ProgressBar.hpp>

namespace Tether::Rendering::Elements
{
	ProgressBar::ProgressBar(WindowUI& windowUI)
		:
		Element(windowUI),
		m_BackgroundRect(m_Renderer.CreateObject<Objects::Rectangle>()),
		m_BorderRect(m_Renderer.CreateObject<Objects::Rectangle>()),
		m_ProgressRect(m_Renderer.CreateObject<Objects::Rectangle>())
	{
		m_Objects.push_back(*m_BorderRect);
		m_Objects.push_back(*m_BackgroundRect);
		m_Objects.push_back(*m_ProgressRect);
	}

	void ProgressBar::SetProgress(float progress)
	{
		m_Progress = progress;
		UpdateTransform();
	}

	void ProgressBar::SetMaxProgress(float maxProgress)
	{
		m_MaxProgress = maxProgress;
		UpdateTransform();
	}

	void ProgressBar::UpdateTransform()
	{
		m_BorderRect->SetX(m_X - m_BorderSize);
		m_BorderRect->SetY(m_Y - m_BorderSize);
		m_BorderRect->SetWidth(m_Width + m_BorderSize * 2.0f);
		m_BorderRect->SetHeight(m_Height + m_BorderSize * 2.0f);

		m_BackgroundRect->SetX(m_X);
		m_BackgroundRect->SetY(m_Y);
		m_BackgroundRect->SetWidth(m_Width);
		m_BackgroundRect->SetHeight(m_Height);

		m_ProgressRect->SetX(m_X);
		m_ProgressRect->SetY(m_Y);
		m_ProgressRect->SetWidth((m_Progress / m_MaxProgress) * m_Width);
		m_ProgressRect->SetHeight(m_Height);
	}

	void ProgressBar::UpdateStyle()
	{
		m_BorderRect->SetColor(m_BorderColor);
		m_BackgroundRect->SetColor(m_BackgroundColor);
		m_ProgressRect->SetColor(Math::Vector4f(0.0f, 1.0f, 0.2f, 1.0f));
	}
}