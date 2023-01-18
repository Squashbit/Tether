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

		virtual void OnObjectUpdate() {}
	};

	class TETHER_EXPORT Object
	{
	public:
		virtual ~Object() = default;
		TETHER_NO_COPY(Object);

		Renderer* GetUIRenderer();
		ObjectRenderer* GetObjectRenderer();
	protected:
		Object(Renderer* pRenderer);

		Renderer* pRenderer = nullptr;
		ObjectRenderer* pObjectRenderer = nullptr;
	};
}