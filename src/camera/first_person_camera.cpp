#include "lvutils/camera/first_person_camera.hpp"

#include <iostream>

namespace lv {

void FirstPersonCamera::inputs(LvndWindow* window, float dt, int mouseX, int mouseY, int width, int height) {
	//Movement
	/*
	float speed = initSpeed * dt;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		speed *= 1.6f;
	}
	*/
	if (lvndGetKeyState(window, LVND_KEY_W) == LVND_STATE_PRESSED) {
		movement += direction;
	}
	if (lvndGetKeyState(window, LVND_KEY_A) == LVND_STATE_PRESSED) {
		movement += -glm::normalize(glm::cross(direction, up));
	}
	if (lvndGetKeyState(window, LVND_KEY_S) == LVND_STATE_PRESSED) {
		movement += -direction;
	}
	if (lvndGetKeyState(window, LVND_KEY_D) == LVND_STATE_PRESSED) {
		movement += glm::normalize(glm::cross(direction, up));
	}
	/*
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		movement += speed * up;
	}
	*/

	if (lvndGetKeyState(window, LVND_KEY_ESCAPE) == LVND_STATE_PRESSED) {
		// Hides mouse cursor
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		// Prevents camera from jumping on the first click
		if (firstClick) {
			firstClick = false;
			drag = !drag;
			mouseX = (width * 0.5f);
			mouseY = (height * 0.5f);
			if (drag) {
				lvndSetCursorState(window, LVND_CURSOR_STATE_HIDDEN);
			} else {
				lvndSetCursorState(window, LVND_CURSOR_STATE_NORMAL);
			}
		}
	} else if (lvndGetKeyState(window, LVND_KEY_ESCAPE) == LVND_STATE_RELEASED) {
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;
	}
	
	if (drag) {
		lvndSetCursorPosition(window, (width * 0.5f), (height * 0.5f));
		//std::cout << "Mouse: " << mouseX << ", " << mouseY << " : " << (width * 0.5f) << ", " << (height * 0.5f) << std::endl;
		float rotX = sensitivity * float(mouseY - (height * 0.5f)) / float(height);
		float rotY = sensitivity * float(mouseX - (width * 0.5f)) / float(width);

		// Calculates upcoming vertical change in the direction
		direction = glm::rotate(direction, glm::radians(rotX), glm::normalize(glm::cross(direction, up)));

		/*
		if (abs(glm::angle(newDirection, up) - glm::radians(90.0f)) <= glm::radians(85.0f)) {
		direction = newDirection;
		}
		*/

		// Rotates the direction left and right
		direction = glm::rotate(direction, glm::radians(-rotY), up);
	}
}

} //namespace lv
