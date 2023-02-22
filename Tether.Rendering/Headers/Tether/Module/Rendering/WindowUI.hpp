#pragma once

#include <Tether/Window.hpp>

#include <Tether/Module/Rendering/Elements/Element.hpp>
#include <Tether/Module/Rendering/Compositor.hpp>
#include <Tether/Module/Rendering/Renderer.hpp>

#include <Tether/Math/Types.hpp>

namespace Tether::Rendering
{
	class TETHER_EXPORT WindowUI
	{
	public:
		WindowUI(Window& window, Renderer& renderer);
		WindowUI(Window& window, Renderer& renderer, Compositor& compositor);
		~WindowUI();
		TETHER_NO_COPY(WindowUI);
		
		void AddElement(Elements::Element& element, bool repaint = true);
		bool RemoveElement(Elements::Element& element, bool repaint = true);

		/**
		 * @brief 
		 * Removes all elements from the GraphicalWindow.
		 */
		void ClearElements();
		
		/**
		 * @brief Sets the background color of the window.
		 */
		void SetBackgroundColor(Math::Vector4f backgroundColor);

		Renderer& GetRenderer();
	private:
		void Repaint();
		
		class Repainter : public Events::EventHandler
		{
		public:
			Repainter(WindowUI& windowUI);

			void OnWindowRepaint(Events::WindowRepaintEvent event) override;
		private:
			WindowUI& m_WindowUI;
		};

		std::vector<Elements::Element*> m_Elements;

		Renderer& m_Renderer;

		std::optional<Repainter> m_Repainter;
		Compositor* m_Compositor = nullptr;

		Window& m_Window;
	};
}
