#pragma once

#include <Tether/Window.hpp>

#include <Tether/Rendering/Elements/Element.hpp>
#include <Tether/Rendering/GraphicsContext.hpp>
#include <Tether/Rendering/Compositor.hpp>

#include <Tether/Math/Types.hpp>

namespace Tether::Rendering
{
	class TETHER_EXPORT WindowUIManager
	{
		friend Elements::Element;
	public:
		WindowUIManager(Window& window);
		~WindowUIManager();
		TETHER_NO_COPY(WindowUIManager);

		void SetAutoRepaint(bool autoRepaint);
		
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
		
		class Repainter : public Events::EventHandler
		{
		public:
			Repainter(WindowUIManager& windowUI);

			void OnWindowRepaint() override;
			void OnWindowResize(const Events::WindowResizeEvent& event) override;
		private:
			WindowUIManager& m_WindowUI;
		};

		Repainter m_Repainter;
		
		std::vector<Elements::Element*> m_Elements;
	};

	class TETHER_EXPORT ScopedNoRepaint
	{
	public:
		ScopedNoRepaint(WindowUIManager& windowUI);
		~ScopedNoRepaint();
	private:
		WindowUIManager& m_WindowUI;
	};
}
