#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "lve_device.hpp"

#include <cstdio>
#include <cstdlib>

namespace lve
{
	


		static void check_vk_result(VkResult err);
		void init_imgui(GLFWwindow* window, VkFormat format, VkDescriptorPool descriptorPool, LveDevice& lveDevice);
		void draw_imgui();
		static VkRenderPass renderPass;
}