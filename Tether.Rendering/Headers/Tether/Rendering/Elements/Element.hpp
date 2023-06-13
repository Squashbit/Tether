#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Math/Types.hpp>

#include <Tether/Window.hpp>

namespace Tether::Rendering
{
	class WindowUIManager;
}

namespace Tether::Rendering::Elements
{
	class TETHER_EXPORT Element
	{
		friend WindowUIManager;
	public:
		Element(GraphicsContext& graphicsContext);
		Element(GraphicsContext& graphicsContext, WindowUIManager& windowUI);
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
		void Repaint();

		bool m_IsInWindowUI = false;

		float m_X = 0.0f;
		float m_Y = 0.0f;
		float m_Width = 0.0f;
		float m_Height = 0.0f;
		Math::Vector4f m_Color;
		Math::Vector4f m_BackgroundColor;

		GraphicsContext& m_GraphicsContext;
	private:
		WindowUIManager* m_pWindowUI = nullptr;
	};
}
