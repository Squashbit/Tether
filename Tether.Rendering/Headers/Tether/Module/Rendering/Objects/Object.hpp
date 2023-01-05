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
		Object(UIRenderer* pRenderer, Scope<ObjectRenderer> pObjectRenderer);
		Object(Object&&) noexcept;

		Object(const Object&) = delete;
		Object& operator=(const Object&) = delete;
		Object& operator=(Object&&) = delete;

		UIRenderer* GetUIRenderer();
		ObjectRenderer* GetObjectRenderer();
	protected:
		UIRenderer* pRenderer = nullptr;
		Scope<ObjectRenderer> pObjectRenderer;
	};
}