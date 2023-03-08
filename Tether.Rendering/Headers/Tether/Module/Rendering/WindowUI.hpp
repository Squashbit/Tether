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
		friend Elements::Element;
	public:
		WindowUI(Window& window, Renderer& renderer);
		WindowUI(Window& window, Renderer& renderer, Compositor& compositor);
		~WindowUI();
		TETHER_NO_COPY(WindowUI);

		void SetAutoRepaint(bool autoRepaint);
		void SetRepaintOnResize(bool repaintOnResize);
		
		void AddElement(Elements::Element& element);
		bool RemoveElement(Elements::Element& element);

		/**
		 * @brief 
		 * Removes all elements from the GraphicalWindow.
		 */
		void ClearElements();
		
		/**
		 * @brief Sets the background color of the window.
		 */
		void SetBackgroundColor(Math::Vector4f backgroundColor);

		void Repaint(bool isAutomatic = false);

		Window& GetWindow();
		Renderer& GetRenderer();
	private:
		bool m_AutoRepaint = true;
		bool m_RepaintOnResize = false;

		class Repainter : public Events::EventHandler
		{
		public:
			Repainter(WindowUI& windowUI);

			void OnWindowRepaint(Events::WindowRepaintEvent event) override;
			void OnWindowResize(Events::WindowResizeEvent event) override;
		private:
			WindowUI& m_WindowUI;
		};

		std::vector<Elements::Element*> m_Elements;

		Renderer& m_Renderer;

		std::optional<Repainter> m_Repainter;
		Compositor* m_Compositor = nullptr;

		Window& m_Window;
	};

	class TETHER_EXPORT ScopedNoRepaint
	{
	public:
		ScopedNoRepaint(WindowUI& windowUI);
		~ScopedNoRepaint();
	private:
		WindowUI& m_WindowUI;
	};
}
