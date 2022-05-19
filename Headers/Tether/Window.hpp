/*
 * The Window class uses OpenGL to draw graphics. As a result, it can
 * draw far more complicated graphics then the NativeWindow class.
 * 
 * NOTICE:
 * This file is still in preview. If you want to use it, you have to compile tether
 * with the TETHER_PREVIEW_FEATURES option enabled.
 */

#ifndef _TETHER_WINDOW_HPP
#define _TETHER_WINDOW_HPP

#include <Tether/IWindow.hpp>
#include <Tether/Elements/Element.hpp>

namespace Tether
{
	class Window : public IWindow
	{
	public:
		Window() {}
		TETHER_NO_COPY(Window);

		/**
		 * @brief 
		 * Adds an element to the window.
		 * 
		 * @param pElement A pointer to the element to add.
		 * @param repaint Optional. If true, will repaint the Window when the element
		 *  is added. If false, the element will be added but until repaint is called,
		 *  the element won't be visible.
		 */
		void AddElement(Elements::Element* pElement, bool repaint = true);
		/**
		 * @brief 
		 * Removes an element from the window.
		 *
		 * @param pElement A pointer to the element to add.
		 * @param repaint Optional. If true, will repaint the Window when the element
		 *  is removed. If false, the element will be removed but until repaint is 
		 *  called, the element will still be visible.
		 */
		bool RemoveElement(Elements::Element* pElement, bool repaint = true);

		/**
		 * @brief 
		 * Removes all elements from the Window.
		 */
		void ClearElements();
		
		/**
		 * @brief Sets the background color of the window.
		 */
		void SetBackgroundColor(Color backgroundColor);
		
		void Repaint();
	private:
		bool InitGraphics();
		void DisposeGraphics();
		
		std::vector<Elements::Element*> elements;
		
		Color backgroundColor;
	};
}

#endif //_TETHER_WINDOW_HPP