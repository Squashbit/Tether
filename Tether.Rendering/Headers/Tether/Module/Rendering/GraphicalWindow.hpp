#pragma once

#include <Tether/Module/Rendering/Common/Defs.hpp>

#include <Tether/Elements/Element.hpp>
#include <Tether/Math/Types.hpp>
#include <Tether/SimpleWindow.hpp>

namespace Tether::Rendering
{
	class TETHER_EXPORT GraphicalWindow : public SimpleWindow
	{
	public:
		GraphicalWindow(int width, int height, const char* title, bool visible = false);
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
		void SetBackgroundColor(Math::Vector4f backgroundColor);
		
		void Repaint();
	private:
		bool InitGraphics();
		void DisposeGraphics();
		
		std::vector<Elements::Element*> elements;
		
		Math::Vector4f backgroundColor;
	};
}
