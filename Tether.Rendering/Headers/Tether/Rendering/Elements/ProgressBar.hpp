#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Rendering/Elements/Element.hpp>
#include <Tether/Rendering/Elements/Common/BorderedElement.hpp>

#include <atomic>

namespace Tether::Rendering::Elements
{
	class TETHER_EXPORT ProgressBar : public Element, public BorderedElement
	{
	public:
		ProgressBar(WindowUIManager& windowUI);

		void SetProgress(float progress);
		void SetMaxProgress(float maxProgress);
		void SetProgressColor(Math::Vector4f color);
	private:
		void OnRender(Renderer& renderer) override;
		
		std::atomic<float> m_Progress = 0.0f;
		std::atomic<float> m_MaxProgress = 100.0f;

		Math::Vector4f m_ProgressColor = Math::Vector4f(0.0f, 1.0f, 0.2f, 1.0f);
	};
}
