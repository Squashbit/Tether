/*
 * The GraphicalWindow class uses OpenGL to draw graphics. As a result, it can
 * draw far more complicated graphics then the NativeWindow class.
 */

#ifndef _TETHER_GRAPHICALWINDOW_HPP
#define _TETHER_GRAPHICALWINDOW_HPP

#include <Tether/Module/Rendering/Common/Defs.hpp>

#include <Tether/Common/Color.hpp>
#include <Tether/Elements/Element.hpp>
#include <Tether/SimpleWindow.hpp>

namespace Tether::Rendering
{
	class TETHER_EXPORT GraphicalWindow : public SimpleWindow
	{
	public:
		GraphicalWindow() {}
		TETHER_NO_COPY(GraphicalWindow);

		/**
		 * @brief 
		 * Adds an element to the window.
		 * 
		 * @param pElement A pointer to the element to add.
		 * @param repaint Optional. If true, will repaint the GraphicalWindow when the element
		 *  is added. If false, the element will be added but until repaint is called,
		 *  the element won't be visible.
		 */
		void AddElement(Elements::Element* pElement, bool repaint = true);
		/**
		 * @brief 
		 * Removes an element from the window.
		 *
		 * @param pElement A pointer to the element to add.
		 * @param repaint Optional. If true, will repaint the GraphicalWindow when the element
		 *  is removed. If false, the element will be removed but until repaint is 
		 *  called, the element will still be visible.
		 */
		bool RemoveElement(Elements::Element* pElement, bool repaint = true);

		/**
		 * @brief 
		 * Removes all elements from the GraphicalWindow.
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

#endif //_TETHER_GRAPHICALWINDOW_HPP