#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Math/Types.hpp>

#include <Tether/Window.hpp>

#include <Tether/Module/Rendering/Renderer.hpp>

namespace Tether::Rendering
{
	class WindowUI;
}

namespace Tether::Rendering::Elements
{
	class TETHER_EXPORT Element
	{
		friend WindowUI;
	public:
		Element(WindowUI& windowUI);
		virtual ~Element() = 0;

		void SetX(float x);
		void SetY(float y);
		void SetWidth(float width);
		void SetHeight(float height);
		void SetBorderSize(float borderSize);
		void SetColor(Math::Vector4f color);
		void SetBackgroundColor(Math::Vector4f color);
		void SetBorderColor(Math::Vector4f color);

		float GetX() const;
		float GetY() const;
		float GetWidth() const;
		float GetHeight() const;
		Math::Vector4f GetColor() const;
		Math::Vector4f GetBackgroundColor() const;
	protected:
		bool m_IsInWindowUI = false;

		void OnAdd();
		void OnRemove();

		virtual void UpdateTransform() {}
		virtual void UpdateStyle() {}

		float m_X = 0.0f;
		float m_Y = 0.0f;
		float m_Width = 0.0f;
		float m_Height = 0.0f;
		float m_BorderSize = 0.0f;
		Math::Vector4f m_Color;
		Math::Vector4f m_BackgroundColor;
		Math::Vector4f m_BorderColor;

		WindowUI& m_WindowUI;
		Window& m_Window;
		Renderer& m_Renderer;
		
		std::vector<std::reference_wrapper<Objects::Object>> m_Objects;
	private:
		void ChangeTransform();
		void ChangeStyle();
	};
}
