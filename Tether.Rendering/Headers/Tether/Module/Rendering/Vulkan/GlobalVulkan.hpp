#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/Ref.hpp>

namespace Tether::Rendering::Vulkan
{
	struct VulkanNative;
	class DebugCallback;

	class TETHER_EXPORT GlobalVulkan
	{
	public:
		~GlobalVulkan();
		TETHER_NO_COPY(GlobalVulkan);

		Vulkan::VulkanNative* GetVulkanNative();

		// The DebugCallback object must be alive until it is removed.
		void AddDebugMessenger(DebugCallback* pDebugCallback);
		void RemoveDebugMessenger(DebugCallback* pDebugCallback);
		
		static void Create(bool debugMode);
		static bool IsAlive();
		static void Destroy();
		static GlobalVulkan& Get();
	private:
		GlobalVulkan(bool debugMode);

		void LoadVulkan();

		Vulkan::VulkanNative* vulkan = nullptr;

		static GlobalVulkan* internal;
	};
}
