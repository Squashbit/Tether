#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Math/Types.hpp>

#include <Tether/Window.hpp>
#include <Tether/Rendering/WindowUIManager.hpp>

namespace Tether::Rendering::Elements
{
	class BorderedElement;

	class TETHER_EXPORT Element
	{
		friend WindowUIManager;
		friend BorderedElement;
	public:
		Element(WindowUIManager& windowUI);
		virtual ~Element() = 0;

		void SetX(float x);
		void SetY(float y);
		void SetWidth(float width);
		void SetHeight(float height);
		void SetColor(Math::Vector4f color);
		void SetBackgroundColor(Math::Vector4f color);

		float GetX() const;
		float GetY() const;
		float GetWidth() const;
		float GetHeight() const;
		Math::Vector4f GetColor() const;
		Math::Vector4f GetBackgroundColor() const;
	protected:
		virtual void OnRender(Renderer& renderer) = 0;

		void Repaint();

		bool m_IsInWindowUI = false;

		float m_X = 0.0f;
		float m_Y = 0.0f;
		float m_Width = 0.0f;
		float m_Height = 0.0f;
		Math::Vector4f m_Color;
		Math::Vector4f m_BackgroundColor;

		Window& m_Window;
	private:
		WindowUIManager& m_WindowUI;
	};
}
