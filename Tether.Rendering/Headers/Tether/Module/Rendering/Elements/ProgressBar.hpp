#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/WindowUI.hpp>

#include <Tether/Module/Rendering/Objects/Rectangle.hpp>
#include <Tether/Module/Rendering/Elements/Element.hpp>
#include <Tether/Module/Rendering/Elements/Common/BorderedElement.hpp>

#include <atomic>

namespace Tether::Rendering::Elements
{
	class TETHER_EXPORT ProgressBar : public Element, public BorderedElement
	{
	public:
		ProgressBar(WindowUI& windowUI);

		void SetProgress(float progress);
		void SetMaxProgress(float maxProgress);
	private:
		void UpdateTransform();
		void UpdateStyle();
		
		Scope<Objects::Rectangle> m_ProgressRect;

		std::atomic<float> m_Progress = 0.0f;
		std::atomic<float> m_MaxProgress = 100.0f;
	};
}
