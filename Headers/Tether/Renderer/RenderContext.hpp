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
	class IRenderContextNative
	{
	public:
		virtual ~IRenderContextNative() {}
	};

	class RenderContext : public IDisposable
	{
	public:
		RenderContext();
		TETHER_DISPOSE_ON_DESTRUCT(RenderContext);
		TETHER_NO_COPY(RenderContext);

#ifdef TETHER_INCLUDE_VULKAN
		bool CreateVulkanRenderer(VkInstance* pInstance, VkDevice* pDevice);
#endif //TETHER_INCLUDE_VULKAN

		// bool CreateOpenGLRenderer();

		IRenderContextNative *const GetNative() const;
	private:
		void DisposeVulkan();
		// void DisposeOpenGL();

		void OnDispose();

		IRenderContextNative* native = nullptr;
	};
}

#endif //_TETHER_RENDERER_RENDERCONTEXT_HPP