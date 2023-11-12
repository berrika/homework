#pragma once

#include "lve_camera.h"
#include "lve_pipeline.h"
#include "lve_device.hpp"
#include "lve_game_object.h"
#include "lve_frame_info.h"

#include <memory>

namespace lve
{
	struct SimplePushConstantData
	{
		glm::mat4 modelMatrix{1.f};
		glm::mat4 normalMatrix{1.f};
	};
	class SimpleRenderSystem
	{
	public:
		SimpleRenderSystem(LveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(FrameInfo& framInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		//order matter
		LveDevice& lveDevice;

		std::unique_ptr<LvePipeline> lvePipeline;
		VkPipelineLayout pipelineLayout;
	};
}