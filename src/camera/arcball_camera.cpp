#include "lvutils/camera/arcball_camera.hpp"

namespace lv {

void ArcballCamera::inputs(LvndWindow* window, float dt, int mouseX, int mouseY, int width, int height) {
	if (lvndGetMouseButtonState(window, LVND_MOUSE_BUTTON_LEFT) == LVND_STATE_PRESSED) {
		if (firstClick) {
			startMouseX = mouseX;
			startMouseY = mouseY;
			prevMouseX = mouseX;
			prevMouseY = mouseY;
			//std::cout << startMouseX << ", " << startMouseY << std::endl;
			//std::cout << "Resetting..." << std::endl;
		}
		firstClick = false;

		if (!drag) {
			float pixelDistance = sqrt(pow(mouseX - startMouseX, 2) + pow(mouseY - startMouseY, 2));
			drag = (pixelDistance > 10.0f);
		}

		if (drag) {
			//std::cout << mouseX - prevMouseX << ", " << mouseY - prevMouseY << std::endl;
			float rotX = sensitivity * float(mouseY - prevMouseY) / float(height);
			float rotY = sensitivity * float(mouseX - prevMouseX) / float(width);

			// Calculates upcoming vertical change in the direction
			//direction = glm::rotate(direction, glm::radians(rotX), glm::normalize(glm::cross(direction, up)));

			// Decides whether or not the next vertical direction is legal or not
			/*
			if (abs(glm::angle(newDirection, up) - glm::radians(90.0f)) <= glm::radians(85.0f)) {
				direction = newDirection;
			}
			*/

			// Rotates the direction left and right
			//direction = glm::rotate(direction, glm::radians(-rotY), up);
			rotation.x -= rotX;
			rotation.y -= rotY;
		}
	} else {
		firstClick = true;
		drag = false;
	}

	prevMouseX = mouseX;
	prevMouseY = mouseY;

	position = target - glm::toMat3(glm::quat(glm::radians(rotation))) * glm::vec3(0.0f, 0.0f, yScroll);
}

} //namespace lv
