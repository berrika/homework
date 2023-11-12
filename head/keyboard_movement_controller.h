#pragma once

#include "lve_game_object.h"
#include "lve_window.h"

#include <glm/glm.hpp>
#include <iostream>

namespace lve
{
	class KeyboardMovementController
	{
	public:
		struct  KeyMappings
		{
			int moveLeft = GLFW_KEY_A;
			int moveRight = GLFW_KEY_D;
			int moveForward = GLFW_KEY_W;
			int moveBackward = GLFW_KEY_S;
			int moveUp = GLFW_KEY_E;
			int moveDown = GLFW_KEY_Q;
			int LookLeft = GLFW_KEY_LEFT;
			int LookRight = GLFW_KEY_RIGHT;
			int LookUp = GLFW_KEY_UP;
			int LookDown = GLFW_KEY_DOWN;
		};

		void ProcessMouseMovement(GLFWwindow* window, float dt, LveGameObject& gameObject);
		static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
		static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		void initMouse(GLFWwindow* window);
		void moveInPlaneXZ(GLFWwindow* window, float dt, LveGameObject& gameObject);

		KeyMappings keys{};
		float moveSpeed{ 3.f };
		float lookSpeed{ 1.5f };
		static bool state;
	};
}