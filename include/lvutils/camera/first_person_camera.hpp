#ifndef LV_FIRST_PERSON_CAMERA_H
#define LV_FIRST_PERSON_CAMERA_H

#include <cmath>

#define LVND_DEBUG
#include "lvnd/lvnd.h"

#include "camera.hpp"

namespace lv {

class FirstPersonCamera : public CameraComponent {
public:
	//Positions
	//glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
	float distance = 3.0f;
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 movement = glm::vec3(0.0f);

	//Look around
	bool firstClick = true;
	bool drag = false;

	// Adjust the speed of the camera and it's sensitivity when looking around
	//float initSpeed = 6.0f;
	float sensitivity = 100.0f;
	//float scrollSensitivity = 1.0f;

  	void inputs(LvndWindow* window, float dt, int mouseX, int mouseY, int width, int height);
};

} //namespace lv

#endif
