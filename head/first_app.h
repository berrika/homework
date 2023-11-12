#pragma once

#include "lve_window.h"
#include "simple_render_system.h"
#include "lve_device.hpp"
#include "lve_game_object.h"
#include "lve_renderer.h"
#include "lve_descriptors.h"
//#include "lve_gui.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <filesystem>
namespace lve
{
	class FirstApp
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;
		void run();

		void init_imgui(GLFWwindow* window, VkFormat format, VkDescriptorPool descriptorPool);
		void draw_imgui();
		std::string path1;
		std::string path2;
	private:
		void loadGameObjects();
		//order matter
		LveWindow lveWindow{ WIDTH, HEIGHT, "VBM" };
		LveDevice lveDevice{ lveWindow };
		LveRenderer lveRenderer{ lveWindow, lveDevice };
		//order of declarations matters
		std::unique_ptr<LveDescriptorPool> globalPool{};
		LveGameObject::Map gameObjects;
	};
}
