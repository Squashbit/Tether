#include <Tether/Module/Rendering/Elements/ProgressBar.hpp>

namespace Tether::Rendering::Elements
{
	ProgressBar::ProgressBar(WindowUI& windowUI)
		:
		Element(windowUI),
		BorderedElement(windowUI, *this),
		m_ProgressRect(m_Renderer.CreateObject<Objects::Rectangle>())
	{
		AddBorderObjects(m_Objects);

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
		UpdateBorderTransform();

		m_ProgressRect->SetX(m_X);
		m_ProgressRect->SetY(m_Y);
		m_ProgressRect->SetWidth((m_Progress / m_MaxProgress) * m_Width);
		m_ProgressRect->SetHeight(m_Height);
	}

	void ProgressBar::UpdateStyle()
	{
		UpdateBorderStyle();

		m_ProgressRect->SetColor(Math::Vector4f(0.0f, 1.0f, 0.2f, 1.0f));
	}
}