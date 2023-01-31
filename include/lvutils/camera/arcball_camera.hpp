#ifndef LV_ARCBALL_CAMERA_H
#define LV_ARCBALL_CAMERA_H

#include <cmath>

#define LVND_DEBUG
#include "lvnd/lvnd.h"

#include "camera.hpp"

namespace lv {

class ArcballCamera : public CameraComponent {
public:
  	//Positions
  	glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);

  	//Look around
	bool firstClick = true;
	bool drag = false;

	float yScroll = 1.0f;
	int prevMouseX = 0, prevMouseY = 0;
	int startMouseX = 0, startMouseY = 0;

	// Adjust the speed of the camera and it's sensitivity when looking around
	//float initSpeed = 3.f;
	float sensitivity = 100.0f;
  	float scrollSensitivity = 1.0f;
	
  	void inputs(LvndWindow* window, float dt, int mouseX, int mouseY, int width, int height);
};

} //namespace lv

#endif
