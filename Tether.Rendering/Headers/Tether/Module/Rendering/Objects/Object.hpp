#pragma once

#include <Tether/Common/Ref.hpp>
#include <Tether/Common/Defs.hpp>

namespace Tether::Rendering
{
	class Renderer;
}

namespace Tether::Rendering::Objects
{
	class TETHER_EXPORT ObjectRenderer
	{
	public:
		virtual ~ObjectRenderer() = default;
	};

	class TETHER_EXPORT Object
	{
		friend Renderer;
	public:
		virtual ~Object();
		TETHER_NO_COPY(Object);

		void SetX(float value);
		void SetY(float value);
		void SetEnabled(bool enabled);
		float GetX();
		float GetY();
		bool IsEnabled();
		
		ObjectRenderer& GetObjectRenderer() const;
	protected:
		Object(ObjectRenderer& objectRenderer);

		float x = 0.0f;
		float y = 0.0f;
		bool m_Enabled = true;

		bool m_IsInRenderer = false;
		Renderer* m_pRenderer = nullptr;
	private:
		ObjectRenderer& m_ObjectRenderer;
	};
}