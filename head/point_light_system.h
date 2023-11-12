#pragma once

#include "lve_camera.h"
#include "lve_pipeline.h"
#include "lve_device.hpp"
#include "lve_game_object.h"
#include "lve_frame_info.h"

#include <memory>

namespace lve
{
	class PointLightSystem
	{
	public:
		PointLightSystem(LveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void render(FrameInfo& framInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		//order matter
		LveDevice& lveDevice;

		std::unique_ptr<LvePipeline> lvePipeline;
		VkPipelineLayout pipelineLayout;
	};
}