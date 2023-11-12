#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <vulkan/vulkan.h>

namespace lve
{

	class LveWindow
	{
	public:
		LveWindow(int w, int h, std::string name);
		~LveWindow();
		
		LveWindow(const LveWindow&) = delete;
		LveWindow& operator=(const LveWindow&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
		bool wasWindowResized() { return frambufferResized; }
		void resetWindowResizedFlag() { frambufferResized = false; }
		GLFWwindow* getGLFWwindow() const { return window; }

		const int getHeight() const { return height; }
		const int getWidth() const { return width; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
		//void setupImguiContext();

	private:
		static void frambufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow();

		int width;
		int height;
		bool frambufferResized = false;

		std::string windowName;
		GLFWwindow* window;
	};
}