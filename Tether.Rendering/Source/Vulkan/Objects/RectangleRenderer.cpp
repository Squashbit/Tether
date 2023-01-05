#include <Tether/Module/Rendering/Vulkan/Objects/RectangleRenderer.hpp>
#include <Tether/Module/Rendering/RendererException.hpp>
#include <iostream>

using namespace Tether::Rendering::Vulkan;

RectangleRenderer::RectangleRenderer(VulkanUIRenderer* pVkRenderer,
	Objects::Rectangle* pRectangle)
	:
	ObjectRenderer(pVkRenderer),
	pRectangle(pRectangle)
{
	this->device = pVkRenderer->GetDevice();
	this->dloader = device->GetLoader();
	this->pRectBuffer = pVkRenderer->GetRectangleBuffer();
	this->allocator = pVkRenderer->GetAllocator();

	uint32_t imageCount = pVkRenderer->GetSwapchainImageCount();

	uniformBuffers.resize(imageCount);
	uniformAllocations.resize(imageCount);
	uniformAllocInfos.resize(imageCount);

	for (size_t i = 0; i < imageCount; i++)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = sizeof(transform);
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		if (vmaCreateBuffer(
			allocator, &bufferInfo, &allocInfo, 
			&uniformBuffers[i], &uniformAllocations[i], 
			&uniformAllocInfos[i]) != VK_SUCCESS)
			throw RendererException("Failed to create staging buffer");
	}

	CreateDescriptorPool();
	CreateDescriptorSets();
}

RectangleRenderer::~RectangleRenderer()
{
	pVkRenderer->WaitForCommandBuffers();

	for (size_t i = 0; i < pVkRenderer->GetSwapchainImageCount(); i++)
		vmaDestroyBuffer(allocator, uniformBuffers[i], uniformAllocations[i]);

	dloader->vkDestroyDescriptorPool(device->Get(), descriptorPool, nullptr);
}

void RectangleRenderer::CreateDescriptorPool()
{
	uint32_t imageCount = pVkRenderer->GetSwapchainImageCount();

	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = imageCount;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = imageCount;

	if (dloader->vkCreateDescriptorPool(device->Get(), &poolInfo, nullptr,
		&descriptorPool) != VK_SUCCESS)
		throw RendererException("Failed to create Vulkan descriptor pool");
}

void RectangleRenderer::CreateDescriptorSets()
{
	uint32_t imageCount = pVkRenderer->GetSwapchainImageCount();

	std::vector<VkDescriptorSetLayout> layouts(
		imageCount,
		pVkRenderer->GetDescriptorSetLayout()
	);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = imageCount;
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(layouts.size());
	if (dloader->vkAllocateDescriptorSets(device->Get(), &allocInfo,
		descriptorSets.data()) != VK_SUCCESS)
		throw RendererException("Failed to allocate descriptor sets");

	for (size_t i = 0; i < layouts.size(); i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(Transform);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		dloader->vkUpdateDescriptorSets(device->Get(), 1, &descriptorWrite, 0, nullptr);
	}
}

void RectangleRenderer::OnObjectUpdate()
{
	transform.position.x = pRectangle->GetX();
	transform.position.y = pRectangle->GetY();
	transform.scale.x = pRectangle->GetWidth();
	transform.scale.y = pRectangle->GetHeight();

	for (size_t i = 0; i < pVkRenderer->GetSwapchainImageCount(); i++)
		memcpy(uniformAllocInfos[i].pMappedData, &transform,
			sizeof(Transform));
}

void RectangleRenderer::AddToCommandBuffer(VkCommandBuffer commandBuffer,
	uint32_t index)
{
	VkBuffer vbuffers[] = { pRectBuffer->GetBuffer() };
	VkDeviceSize offsets[] = { 0 };
	dloader->vkCmdBindVertexBuffers(commandBuffer, 0, 1, vbuffers, offsets);
	dloader->vkCmdBindIndexBuffer(commandBuffer, pRectBuffer->GetIndexBuffer(), 0,
		VK_INDEX_TYPE_UINT32);

	dloader->vkCmdBindDescriptorSets(
		commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		pVkRenderer->GetPipeline()->GetLayout(), 0,
		1, &descriptorSets[index],
		0, nullptr
	);

	dloader->vkCmdDrawIndexed(
		commandBuffer,
		static_cast<uint32_t>(pRectBuffer->GetVertexCount()),
		1, 0, 0, 0
	);
}