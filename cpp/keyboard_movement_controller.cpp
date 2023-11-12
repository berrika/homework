#include "keyboard_movement_controller.h"

//std
#include <limits>
//#include <iostream>


static bool firstMouse;
static float lastX;
static float lastY;
float deltaX, deltaY;
bool lve::KeyboardMovementController::state = false;
void lve::KeyboardMovementController::ProcessMouseMovement(GLFWwindow* window, float dt, LveGameObject& gameObject)
{
	glm::vec3 rotate{0};
	rotate.y += deltaX * 0.00005f;
	rotate.x -= deltaY * 0.00005f;
	//avoid to compare float or double with a zero directly 
	if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
		gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
	//limit pitch values between about +/- 85ish degrees
	gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.f);
	gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

}
void lve::KeyboardMovementController::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	deltaX = xpos - lastX;
	deltaY = ypos - lastY;

	lastX = xpos;
	lastY = ypos;
	//std::cout << deltaX << " ," << deltaY << std::endl;
}

void lve::KeyboardMovementController::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		state = true;
	else
		state = false;
}

void lve::KeyboardMovementController::initMouse(GLFWwindow* window)
{
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
}

void lve::KeyboardMovementController::moveInPlaneXZ(GLFWwindow* window, float dt, LveGameObject& gameObject)
{
	//store user input for rotation
	glm::vec3 rotate{0};
	if (glfwGetKey(window, keys.LookRight) == GLFW_PRESS)
		rotate.y += 1.f;
	if (glfwGetKey(window, keys.LookLeft) == GLFW_PRESS)
		rotate.y -= 1.f;
	if (glfwGetKey(window, keys.LookUp) == GLFW_PRESS)
		rotate.x += 1.f;
	if (glfwGetKey(window, keys.LookDown) == GLFW_PRESS)
		rotate.x -= 1.f;
	//avoid to compare float or double with a zero directly 
	if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
		gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
	//limit pitch values between about +/- 85ish degrees
	gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.f);
	gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

	float yaw = gameObject.transform.rotation.y;
	const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
	const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
	const glm::vec3 upDir{0.f, -1.f, 0.f};

	glm::vec3 moveDir{0.f};
	if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS)
		moveDir += forwardDir;
	if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS)
		moveDir -= forwardDir;
	if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS)
		moveDir += rightDir;
	if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS)
		moveDir -= rightDir;
	if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS)
		moveDir += upDir;
	if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS)
		moveDir -= upDir;

	if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
		gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
}

