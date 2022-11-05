#pragma once

#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Common/IDisposable.hpp>
#include <Tether/Common/Types.hpp>

#ifdef TETHER_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#endif // TETHER_INCLUDE_VULKAN

namespace Tether::Renderer
{
	// A "native" is a class that another class can store as a member variable,
	// usually for something platform specific, or API specific,
	// therefore being "native" to that API or Operating System.
	class TETHER_EXPORT IRenderContextNative : public IDisposable
	{
	public:
		IRenderContextNative() = default;
		virtual ~IRenderContextNative() {}
		TETHER_NO_COPY(IRenderContextNative);

		virtual bool RenderFrame() { return false; }
	};

	class TETHER_EXPORT RenderContext : public IDisposable
	{
	public:
		RenderContext();
		TETHER_DISPOSE_ON_DESTRUCT(RenderContext);
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

		bool RenderFrame();

		IRenderContextNative *const GetNative() const;
	private:
		void DisposeVulkan();
		
		void OnDispose();

		IRenderContextNative* native = nullptr;
	};
}
