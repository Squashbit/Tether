#pragma once

#include <Tether/Common/IDisposable.hpp>
#include <Tether/Common/Ref.hpp>
#include <Tether/Common/HashedString.hpp>
#include <Tether/Common/TypeTools.hpp>

#include <Tether/Module/Rendering/Common/Defs.hpp>

namespace Tether::Rendering
{
	class UIRenderer;
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

		UIRenderer* GetUIRenderer();
		ObjectRenderer* GetObjectRenderer();
	protected:
		Object(UIRenderer* pRenderer);

		UIRenderer* pRenderer = nullptr;
		ObjectRenderer* pObjectRenderer = nullptr;
	};
}