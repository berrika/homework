#include "lve_window.h"
#include <stdexcept>

namespace lve
{
	LveWindow::LveWindow(int w, int h, std::string name) : width(w), height(h), windowName(name)
	{
		initWindow();
	}

	LveWindow::~LveWindow()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}


	void LveWindow::initWindow()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this); //set to be the parent lve window object
		glfwSetFramebufferSizeCallback(window, frambufferResizeCallback);
		//setupImguiContext();
	}

	void LveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
			throw std::runtime_error("failed to create window surface!");
	}

	void LveWindow::frambufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto lvewindow = reinterpret_cast<LveWindow*>(glfwGetWindowUserPointer(window));
		lvewindow->frambufferResized = true;
		lvewindow->width = width;
		lvewindow->height = height;
	}
}