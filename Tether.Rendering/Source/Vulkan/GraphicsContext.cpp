#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>
#include <Tether/Rendering/Vulkan/ContextCreator.hpp>
#include <Tether/Rendering/Vulkan/WindowRenderTargetVk.hpp>
#include <stdexcept>

namespace Tether::Rendering::Vulkan
{
	GraphicsContext::GraphicsContext(
		PFN_vkGetInstanceProcAddr GetInstanceProcAddr,

		uint32_t framesInFlight,
		InstanceLoader instanceLoader,
		DeviceLoader deviceLoader,
		VkInstance instance,
		VkDevice device,
		VkQueue queue,
		VkPhysicalDevice physicalDevice,
		VkCommandPool commandPool,
		VmaAllocator allocator
	)
		:
		m_GetInstanceProcAddr(GetInstanceProcAddr),
		m_FramesInFlight(framesInFlight),
		m_InstanceLoader(instanceLoader),
		m_DeviceLoader(deviceLoader),
		m_Instance(instance),
		m_Device(device),
		m_Queue(queue),
		m_PhysicalDevice(physicalDevice),
		m_CommandPool(commandPool),
		m_Allocator(allocator)
	{
		TETHER_ASSERT(
			instance != nullptr &&
			device != nullptr &&
			queue != nullptr &&
			physicalDevice != nullptr &&
			commandPool != nullptr &&
			framesInFlight != 0
		);
		
		if (!m_Allocator)
		{
			m_AllocatorManager.emplace(
				m_GetInstanceProcAddr,
				m_Instance,
				m_InstanceLoader,
				m_Device,
				m_PhysicalDevice
			);

			m_Allocator = m_AllocatorManager->Get();
		}

		CreateVertexBuffers();
		CreateSampler();
	}

	GraphicsContext::GraphicsContext(ContextCreator& vulkanContext)
		:
		m_GetInstanceProcAddr(vulkanContext.GetVulkanLibrary().GetInstanceProcAddr),
		m_FramesInFlight(vulkanContext.GetFramesInFlight()),
		m_InstanceLoader(vulkanContext.GetInstanceLoader()),
		m_DeviceLoader(vulkanContext.GetDeviceLoader()),
		m_Instance(vulkanContext.GetInstance()),
		m_Device(vulkanContext.GetDevice()),
		m_Queue(vulkanContext.GetQueue()),
		m_PhysicalDevice(vulkanContext.GetPhysicalDevice()),
		m_CommandPool(vulkanContext.GetCommandPool())
	{
		m_AllocatorManager.emplace(
			m_GetInstanceProcAddr,
			m_Instance,
			m_InstanceLoader,
			m_Device,
			m_PhysicalDevice
		);

		m_Allocator = m_AllocatorManager->Get();
	}

	GraphicsContext::GraphicsContext(ContextCreator& vulkanContext)
	{

	}

	GraphicsContext::~GraphicsContext()
	{
		m_DeviceLoader.vkDeviceWaitIdle(m_Device);

		m_DeviceLoader.vkDestroySampler(m_Device, sampler, nullptr);
	}

	Scope<RenderTarget> GraphicsContext::CreateWindowRenderTarget(Window& window)
	{
		return std::make_unique<WindowRenderTarget>(*this, window);
	}

	Scope<Resources::BufferedImage> GraphicsContext::CreateBufferedImage(
		const Resources::BufferedImageInfo& info)
	{
		return std::make_unique<BufferedImage>(
			*this, sampler,
			texturedPipelineSetLayout,
			info
		);
	}

	Scope<Resources::Font> GraphicsContext::CreateFont(
		const std::string& fontPath)
	{
		return std::make_unique<Font>(
			*this,
			textPipelineLayout, sampler,
			fontPath
		);
	}

	void GraphicsContext::CreateVertexBuffers()
	{
		Math::Vector2f vertices[] =
		{
			{  0.0f,  0.0f},
			{  1.0f,  0.0f},
			{  1.0f,  1.0f},
			{  0.0f,  1.0f},
		};

		uint32_t indices[] =
		{
			0, 1, 2, 2, 3, 0
		};

		square.emplace(m_Context, sizeof(vertices),
			sizeof(indices) / sizeof(uint32_t));
		square->UploadData(vertices, indices);
		square->FinishDataUpload();
	}

	void GraphicsContext::CreateSampler()
	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 0.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (m_Dloader.vkCreateSampler(m_Context.device, &samplerInfo, nullptr,
			&sampler) != VK_SUCCESS)
			throw std::runtime_error("Failed to create sampler");
	}

	VertexBuffer& GraphicsContext::GetSquareBuffer() const
	{
		return square;
	}

	const uint32_t GraphicsContext::GetFramesInFlight() const
	{
		return m_FramesInFlight;
	}

	const InstanceLoader& GraphicsContext::GetInstanceLoader() const
	{
		return m_InstanceLoader;
	}

	const DeviceLoader& GraphicsContext::GetDeviceLoader() const
	{
		return m_DeviceLoader;
	}

	VkInstance GraphicsContext::GetInstance() const
	{
		return m_Instance;
	}

	VkDevice GraphicsContext::GetDevice() const
	{
		return m_Device;
	}

	VkQueue GraphicsContext::GetQueue() const
	{
		return m_Queue;
	}

	VkPhysicalDevice GraphicsContext::GetPhysicalDevice() const
	{
		return m_PhysicalDevice;
	}

	VkCommandPool GraphicsContext::GetCommandPool() const
	{
		return m_CommandPool;
	}

	VmaAllocator GraphicsContext::GetAllocator() const
	{
		return m_Allocator;
	}
}