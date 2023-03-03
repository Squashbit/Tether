#pragma once

#include <Tether/Module/Rendering/Objects/Object.hpp>
#include <Tether/Module/Rendering/Resources/Font.hpp>

#include <Tether/Math/Types.hpp>

#include <string>

namespace Tether::Rendering::Objects
{
	class TETHER_EXPORT Text : public Object
	{
	public:
		enum class Justify
		{
			LEFT,
			CENTER,
			RIGHT
		};

		virtual ~Text() = default;
		TETHER_NO_COPY(Text);

		void SetText(const std::string& text);
		void SetFont(Resources::Font& font);

		void SetScale(float scale);
		void SetJustify(Justify justify);
		void SetColor(Math::Vector4f color);

		Math::Vector4f GetColor();
	protected:
		Text(ObjectRenderer& objectRenderer);

		std::string m_Text;
		Resources::Font* m_pFont = nullptr;

		float m_Scale = 1.0f;
		Justify m_Justify = Justify::LEFT;
		Math::Vector4f m_Color;
	};
}