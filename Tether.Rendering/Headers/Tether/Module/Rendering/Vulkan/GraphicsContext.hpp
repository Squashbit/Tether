#pragma once

#include <Tether/Module/Rendering/Vulkan/AllocatorManager.hpp>

#include <Tether/Module/Rendering/GraphicsContext.hpp>
#include <Tether/Module/Rendering/Vulkan/VulkanContext.hpp>
#include <Tether/Module/Rendering/Vulkan/VertexBuffer.hpp>

#include <optional>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT GraphicsContext : public Rendering::GraphicsContext
	{
	public:
		/// <summary>
		/// yes
		/// </summary>
		GraphicsContext(
			PFN_vkGetInstanceProcAddr GetInstanceProcAddr,

			uint32_t framesInFlight,
			InstanceLoader instanceLoader, // Must have had Load called
			DeviceLoader deviceLoader, // Must have had Load called
			VkInstance instance,
			VkDevice device,
			VkQueue queue,
			VkPhysicalDevice physicalDevice,
			VkCommandPool commandPool,
			VmaAllocator allocator // One will be created automatically if it is nullptr
		);
		GraphicsContext(VulkanContext& vulkanContext);
		~GraphicsContext();
		TETHER_NO_COPY(GraphicsContext);

		Scope<Objects::Rectangle> CreateRectangleObject() override;
		Scope<Objects::Image> CreateImageObject() override;
		Scope<Objects::Text> CreateTextObject() override;

		Scope<Resources::BufferedImage> CreateBufferedImage(
			const Resources::BufferedImageInfo& info) override;
		Scope<Resources::Font> CreateFont(const std::string& fontPath) override;

		const uint32_t GetFramesInFlight() const;
		const InstanceLoader& GetInstanceLoader() const;
		const DeviceLoader& GetDeviceLoader() const;
		VkInstance GetInstance() const;
		VkDevice GetDevice() const;
		VkQueue GetQueue() const;
		VkPhysicalDevice GetPhysicalDevice() const;
		VkCommandPool GetCommandPool() const;
		VmaAllocator GetAllocator() const;
	private:
		void CreateVertexBuffers();
		void CreateSampler();

		PFN_vkGetInstanceProcAddr m_GetInstanceProcAddr;

		uint32_t m_FramesInFlight = 2;
		InstanceLoader m_InstanceLoader;
		DeviceLoader m_DeviceLoader;
		VkInstance m_Instance = nullptr;
		VkDevice m_Device = nullptr;
		VkQueue m_Queue = nullptr;
		VkPhysicalDevice m_PhysicalDevice = nullptr;
		VkCommandPool m_CommandPool = nullptr;
		VmaAllocator m_Allocator = nullptr;

		std::optional<AllocatorManager> m_AllocatorManager;

		std::optional<VertexBuffer> square;
		VkSampler sampler;
	};
}