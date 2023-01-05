#include "lvutils/camera/camera.hpp"

namespace lv {

void CameraComponent::loadViewProj() {
    direction = glm::toMat3(glm::quat(glm::radians(rotation))) * glm::vec3(0.0f, 0.0f, 1.0f);

    view = glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    
    frustum.create(position, direction, nearPlane, farPlane, fov, aspectRatio);
}

/*
Ray Camera::getRay(float x, float y, float width, float height, float fov, float aspectRatio) {
    x /= width;
    y /= height;

    //Camera view width and height
    float theta = glm::radians(fov);
    float h = tan(theta / 2.0f);
    float viewHeight = 2.0f * h;
    float viewWidth = aspectRatio * viewHeight;

    glm::vec3 w = glm::normalize(-direction);
    glm::vec3 u = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), w));
    glm::vec3 v = glm::cross(w, u);

    glm::vec3 horizontal = viewWidth * u;
    glm::vec3 vertical = viewHeight * v;
    glm::vec3 lowerLeftCorner = position - horizontal / 2.0f - vertical / 2.0f - w;

    return Ray(position, glm::normalize(lowerLeftCorner + x * horizontal + y * vertical - position));
}
*/

/*
bool Camera::isInFrustum(glm::vec3 center, float radius) {
    glm::vec4 viewProjCenter = projection * view * glm::vec4(center, 1.0f);
    glm::vec2 centerInScreen = glm::vec2(viewProjCenter) / viewProjCenter.w;
    if (centerInScreen.x + radius > -1.0f &&
        centerInScreen.x - radius <  1.0f &&
        centerInScreen.y + radius > -1.0f &&
        centerInScreen.y - radius <  1.0f)
        return true;

    return false;

    return true;
}
*/

} //namespace lv
