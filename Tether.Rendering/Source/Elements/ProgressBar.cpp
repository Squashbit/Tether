#include <Tether/Rendering/Elements/ProgressBar.hpp>

namespace Tether::Rendering::Elements
{
	ProgressBar::ProgressBar(WindowUIManager& windowUI)
		:
		Element(windowUI),
		BorderedElement(windowUI, *this)
	{}

	void ProgressBar::SetProgress(float progress)
	{
		m_Progress = progress;
		Repaint();
	}

	void ProgressBar::SetMaxProgress(float maxProgress)
	{
		m_MaxProgress = maxProgress;
		Repaint();
	}

	void ProgressBar::OnRender(Renderer& renderer)
	{
		DrawBorder(renderer);

		// Progress rect
		renderer.FillRect(m_X, m_Y, (m_Progress / m_MaxProgress) * m_Width, 
			m_Height, m_ProgressColor);
	}

	void ProgressBar::SetProgressColor(Math::Vector4f color)
	{
		m_ProgressColor = color;
	}
}