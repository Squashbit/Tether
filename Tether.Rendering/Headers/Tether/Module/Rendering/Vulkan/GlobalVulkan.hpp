#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/Ref.hpp>

#include <Tether/Module/Rendering/Vulkan/DebugCallback.hpp>

#include <Tether/Module/Rendering/Vulkan/Instance.hpp>

#include <optional>

namespace Tether::Rendering::Vulkan
{
	struct VulkanNative;
	class DebugCallback;

	class TETHER_EXPORT GlobalVulkan
	{
	public:
		~GlobalVulkan();
		TETHER_NO_COPY(GlobalVulkan);

		// The DebugCallback object must be alive until it is removed.
		void AddDebugMessenger(DebugCallback& debugCallback);
		void RemoveDebugMessenger(DebugCallback& debugCallback);

		Instance& GetInstance();
		PFN_vkGetInstanceProcAddr GetGetInstanceProcAddr();
		
		static void Create(bool debugMode);
		static bool IsAlive();
		static void Destroy();
		static GlobalVulkan& Get();
	private:
		GlobalVulkan(bool debugMode);

		void LoadVulkan();

		void* handle = nullptr;

		TETHER_VULKAN_FUNC_VAR(CreateInstance);
		TETHER_VULKAN_FUNC_VAR(EnumerateInstanceExtensionProperties);
		TETHER_VULKAN_FUNC_VAR(EnumerateInstanceLayerProperties);
		TETHER_VULKAN_FUNC_VAR(GetInstanceProcAddr);

		std::optional<Instance> instance;

		static GlobalVulkan* internal;
	};
}