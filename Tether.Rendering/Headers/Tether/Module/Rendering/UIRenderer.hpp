#pragma once

#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Objects/Object.hpp>

#include <Tether/Common/IDisposable.hpp>
#include <Tether/Common/TypeTools.hpp>
#include <Tether/Common/HashedString.hpp>
#include <Tether/Common/Types.hpp>

#include <vector>

#ifdef TETHER_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#endif // TETHER_INCLUDE_VULKAN

namespace Tether::Rendering
{
	// A "native" is a class that another class can store as a member variable,
	// usually for something platform specific, or API specific,
	// therefore being "native" to that API or Operating System.
	class UIRenderer;
	class TETHER_EXPORT UIRendererNative : public IDisposable
	{
		friend UIRenderer;
	public:
		UIRendererNative() = default;
		virtual ~UIRendererNative() {}
		TETHER_NO_COPY(UIRendererNative);

		virtual bool OnObjectCreate(HashedString& typeName, Objects::Object* pObject)
		{ return true; }
		virtual void OnObjectAdd(Objects::Object* pObject) {}
		virtual void OnObjectRemove(Objects::Object* pObject) {}

		virtual bool RenderFrame() { return false; }
	protected:
		UIRenderer* pRenderer;
	};

	class TETHER_EXPORT UIRenderer : public IDisposable
	{
		friend UIRendererNative;
	public:
		/**
		 * @param pNative A pointer to the native.
		 * The native must stay alive as long as this Render Context is in use.
		 * The native needed is at Renderer/~api name~/RenderContextNative.hpp.
		 * For example, Vulkan would be located at:
		 * Renderer/Vulkan/RenderContextNative.hpp
		 */
		UIRenderer(UIRendererNative* pNative);
		TETHER_DISPOSE_ON_DESTROY(UIRenderer);
		TETHER_NO_COPY(UIRenderer);

		void AddObject(Objects::Object* pObject);
		bool RemoveObject(Objects::Object* pObject);
		bool HasObject(Objects::Object* pObject);
		void ClearObjects();

		bool RenderFrame();

		template<typename T>
		bool CreateObject(T* pObject)
		{
			HashedString typeHash(TypeTools::GetTypeName<T>());
			return pNative->OnObjectCreate(typeHash, pObject);
		}

		UIRendererNative*const GetNative() const;
		const std::vector<Objects::Object*>& GetObjects() const;
	private:
		void DisposeVulkan();
		
		void OnDispose();

		UIRendererNative* pNative = nullptr;
		std::vector<Objects::Object*> objects;
	};
}
