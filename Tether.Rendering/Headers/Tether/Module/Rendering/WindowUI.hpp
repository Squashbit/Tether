#pragma once

#include <Tether/Window.hpp>

#include <Tether/Module/Rendering/Elements/Element.hpp>
#include <Tether/Module/Rendering/GraphicsContext.hpp>
#include <Tether/Module/Rendering/Compositor.hpp>

#include <Tether/Math/Types.hpp>

namespace Tether::Rendering
{
	class TETHER_EXPORT WindowUI
	{
		friend Elements::Element;
	public:
		WindowUI(Window& window, GraphicsContext& graphicsContext);
		WindowUI(Window& window, GraphicsContext& graphicsContext, Compositor& compositor);
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

		Window& GetWindow() const;
		GraphicsContext& GetGraphicsContext() const;
	private:
		bool m_AutoRepaint = true;
		bool m_RepaintOnResize = false;

		class Repainter : public Events::EventHandler
		{
		public:
			Repainter(WindowUI& windowUI);

			void OnWindowRepaint() override;
			void OnWindowResize(const Events::WindowResizeEvent& event) override;
		private:
			WindowUI& m_WindowUI;
		};

		Window& m_Window;
		GraphicsContext& m_GraphicsContext;

		std::optional<Repainter> m_Repainter;
		Compositor* m_Compositor = nullptr;

		std::vector<Elements::Element*> m_Elements;
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
