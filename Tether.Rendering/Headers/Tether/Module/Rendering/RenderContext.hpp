#pragma once

#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Objects/Object.hpp>

#include <Tether/Common/IDisposable.hpp>
#include <Tether/Common/Types.hpp>

#ifdef TETHER_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#endif // TETHER_INCLUDE_VULKAN

namespace Tether::Rendering
{
	// A "native" is a class that another class can store as a member variable,
	// usually for something platform specific, or API specific,
	// therefore being "native" to that API or Operating System.
	class RenderContext;
	class TETHER_EXPORT IRenderContextNative : public IDisposable
	{
		friend RenderContext;
	public:
		IRenderContextNative() = default;
		virtual ~IRenderContextNative() {}
		TETHER_NO_COPY(IRenderContextNative);

		virtual void OnObjectAdd(Objects::Object* pObject) {}
		virtual void OnObjectRemove(Objects::Object* pObject) {}

		virtual bool RenderFrame() { return false; }

	protected:
		RenderContext* pRenderContext;
	};

	class TETHER_EXPORT RenderContext : public IDisposable
	{
		friend IRenderContextNative;
	public:
		RenderContext() = default;
		TETHER_DISPOSE_ON_DESTROY(RenderContext);
		TETHER_NO_COPY(RenderContext);

		/**
		 * @brief
		 * Initializes the RenderContext (wow really).
		 * 
		 * @param pNative A pointer to the native. 
		 * The native must stay alive as long as this Render Context is in use.
		 * The native needed is at Renderer/~api name~/RenderContextNative.hpp.
		 * For example, Vulkan would be located at:
		 * Renderer/Vulkan/RenderContextNative.hpp
		 */
		bool Init(IRenderContextNative* pNative);

		void Add(Objects::Object* pObject);
		bool Remove(Objects::Object* pObject);
		bool HasObject(Objects::Object* pObject);

		bool RenderFrame();

		IRenderContextNative *const GetNative() const;
		std::vector<Objects::Object*>* GetObjects();
	private:
		void DisposeVulkan();
		
		void OnDispose();

		IRenderContextNative* pNative = nullptr;
		std::vector<Objects::Object*> objects;
	};
}
