#pragma once

#include <Tether/Module.hpp>
#include <Tether/Module/Rendering/Common/Defs.hpp>

namespace Tether::Rendering
{
	namespace Vulkan
	{
		struct VulkanNative;
	}

	class TETHER_EXPORT RenderingModule : public Module
	{
	public:
		TETHER_NO_COPY(RenderingModule);
		RenderingModule() : Module(this, "Tether.Rendering") {}

		/**
		 * @brief
		 * Initializes the Vulkan library. Must be called before Vulkan is used.
		 * 
		 * @param validationLayers True if the Vulkan instance should enable
		 * validation layers.
		 */
		void InitVulkan(bool validationLayers = false);
		bool LoadVulkan();
		void DisposeVulkan();

		Vulkan::VulkanNative* GetVulkanNative();
		bool IsVulkanLoaded();
		bool IsVulkanInitialized();

		static RenderingModule& Get();
	private:
		bool OnInit();
		void OnDispose();

		Vulkan::VulkanNative* vulkan = nullptr;

		static RenderingModule internal;
	};
}
