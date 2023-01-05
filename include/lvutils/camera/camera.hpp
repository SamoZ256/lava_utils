#ifndef LV_CAMERA_H
#define LV_CAMERA_H

#include "frustum.hpp"

/*
#include "Core/UniformBuffer.hpp"
#include "Core/Descriptor.hpp"
*/

/*
#include "Core/Shader.hpp"
#include "../RayTracing/Ray.hpp"
*/

namespace lv {

class CameraComponent {
public:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);
    bool active = false;

    Frustum frustum;

    float width, height = 0;

    //Vectors
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    //Resources
    glm::mat4 projection;
    glm::mat4 view;

    //Projection properties
    float nearPlane = 0.01f;
    float farPlane = 1000.0f;
    float fov = 45.0f;
    float aspectRatio = 1.0f;

    //void uploadUniforms(VkPipelineLayout& pipelineLayout);

    void loadViewProj();

    //Ray getRay(float x, float y, float width, float height, float fov, float aspectRatio);

    //bool isInFrustum(glm::vec3 center, float radius);
};

} //namespace lv

#endif
