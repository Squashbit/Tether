#pragma once

#include <Tether/Common/Ref.hpp>
#include <Tether/Common/Defs.hpp>

namespace Tether::Rendering
{
	class GraphicsContext;
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
	public:
		Object(GraphicsContext& graphicsContext, ObjectRenderer& objectRenderer);
		virtual ~Object() = 0;
		TETHER_NO_COPY(Object);

		void SetX(float value);
		void SetY(float value);
		void SetEnabled(bool enabled);
		float GetX();
		float GetY();
		bool IsEnabled();
		
		ObjectRenderer& GetObjectRenderer() const;
		GraphicsContext& GetGraphicsContext() const;
	protected:
		float x = 0.0f;
		float y = 0.0f;
		bool m_Enabled = true;
	private:
		GraphicsContext& m_GraphicsContext;
		ObjectRenderer& m_ObjectRenderer;
	};
}