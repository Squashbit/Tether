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
		virtual ~Object() = default;
		TETHER_NO_COPY(Object);

		Renderer* GetRenderer();
		ObjectRenderer* GetObjectRenderer();
	protected:
		Object() = default;

		Renderer* m_pRenderer = nullptr;
		ObjectRenderer* m_pObjectRenderer = nullptr;
	};
}