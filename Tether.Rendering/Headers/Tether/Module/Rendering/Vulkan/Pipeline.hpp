#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/IDisposable.hpp>

#include <Tether/Module/Rendering/Vulkan/Common/PipelineInfo.hpp>
#include <Tether/Module/Rendering/Vulkan/DeviceLoader.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT Pipeline : public IDisposable
	{
	public:
		Pipeline() = default;
		TETHER_DISPOSE_ON_DESTRUCT(Pipeline);
		TETHER_NO_COPY(Pipeline);

		/**
		 * @brief
		 * Initializes the Pipeline with shader stages and pInfo which is used to 
		 * specify custom information about the Pipeline.
		 * 
		 * @param pInfo Any data in this structure that isn't 0 or nullptr will
		 *	override the default data set by this class in 
		 *	VkGraphicsPipelineCreateInfo
		 */
		bool Init(VkDevice device, DeviceLoader* dloader, PipelineInfo* pInfo);

		VkPipeline Get();
		VkPipelineLayout GetLayout();
	private:
		void OnDispose();

		DeviceLoader* dloader = nullptr;
		VkDevice device;

		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
	};
}
