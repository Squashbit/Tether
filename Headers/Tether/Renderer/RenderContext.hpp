// WARNING: 
// THIS IS CURRENTLY A PREVIEW FEATURE.
// IN ORDER TO USE THIS PROPERLY, TETHER MUST BE COMPILED WITH THE 
// TETHER_PREVIEW_FEATURES CMAKE OPTION ENABLED.
//
// THIS IS SUBJECT TO CHANGE, AND MAY NOT RESEMBLE THE FINAL VERSION.

#ifndef _TETHER_RENDERER_RENDERCONTEXT_HPP
#define _TETHER_RENDERER_RENDERCONTEXT_HPP

#include <Tether/Common/IDisposable.hpp>
#include <Tether/Common/Defs.hpp>
#include <Tether/Common/Types.hpp>

#ifdef TETHER_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#endif // TETHER_INCLUDE_VULKAN

namespace Tether::Renderer
{
	class IRenderContextNative : public IDisposable
	{
	public:
		IRenderContextNative() = default;
		virtual ~IRenderContextNative() {}
		TETHER_NO_COPY(IRenderContextNative);
	};

	class RenderContext : public IDisposable
	{
	public:
		RenderContext();
		TETHER_DISPOSE_ON_DESTRUCT(RenderContext);
		TETHER_NO_COPY(RenderContext);

		/**
		* @brief
		 * Sets the native for the Render Context (wow really).
		 * A native must be set for the Render Context to function properly.
		 * The native needed is at Renderer/~api name~/RenderContextNative.hpp.
		 * For example, Vulkan would be located at: 
		 * Renderer/Vulkan/RenderContextNative.hpp
		 * 
		 * @param pNative A pointer to the native.
		 */
		bool Init(IRenderContextNative* pNative);

		IRenderContextNative *const GetNative() const;
	private:
		void DisposeVulkan();
		// void DisposeOpenGL();

		void OnDispose();

		IRenderContextNative* native = nullptr;
	};
}

#endif //_TETHER_RENDERER_RENDERCONTEXT_HPP