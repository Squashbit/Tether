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
	};

	class TETHER_EXPORT Object
	{
	public:
		Object(UIRenderer* pRenderer, Scope<ObjectRenderer> pObjectRenderer);
		TETHER_NO_COPY(Object);

		UIRenderer* GetUIRenderer();
		ObjectRenderer* GetObjectRenderer();
	private:
		UIRenderer* pRenderer = nullptr;
		Scope<ObjectRenderer> pObjectRenderer;
	};
}