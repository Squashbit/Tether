#pragma once

#include <Tether/Common/Defs.hpp>
#include <cstdint>

namespace Tether
{
	class TETHER_EXPORT Color
	{
	public:
		Color();
		Color(float color);
		Color(float r, float g, float b);

		/**
		 * @brief Converts a color into a single value
		 */
		static uint32_t ToRGB(float r, float g, float b);

		/**
		 * @brief Converts the color into a single value
		 */
		uint32_t GetRGB() const;

		void SetR(float r);
		void SetG(float g);
		void SetB(float b);
		float GetR() const;
		float GetG() const;
		float GetB() const;
	private:
		float r = 0;
		float g = 0;
		float b = 0;
	};
}
