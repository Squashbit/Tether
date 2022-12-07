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
	class TETHER_EXPORT ObjectNative : public IDisposable
	{
	public:
		ObjectNative() = default;
		virtual ~ObjectNative() {}
		TETHER_NO_COPY(ObjectNative);
	};

	class TETHER_EXPORT Object : public IDisposable
	{
	public:
		Object(UIRenderer* pContext);
		TETHER_DISPOSE_ON_DESTROY(Object);
		TETHER_NO_COPY(Object);

		void SetNative(Scope<ObjectNative>& native);

		UIRenderer* GetUIRenderer();
		ObjectNative* GetNative();
	private:
		UIRenderer* pRenderer = nullptr;
		Scope<ObjectNative> pNative;
	};
}