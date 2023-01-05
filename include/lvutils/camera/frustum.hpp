#ifndef LV_FRUSTUM_H
#define LV_FRUSTUM_H

#include "../libraries/glm.hpp"

namespace lv {

struct Plan {
    glm::vec3 normal = {0.0f, 1.0f, 0.0f};
    float distance = 0.0f;

    Plan() = default;

    Plan(glm::vec3 point, glm::vec3 aNormal) : normal(glm::normalize(aNormal)), distance(glm::dot(normal, point)) {}

    float getSignedDistanceToPlan(const glm::vec3& point);
};

class Frustum {
public:
    Plan topFace;
    Plan bottomFace;
    Plan farFace;
    Plan nearFace;
    Plan rightFace;
    Plan leftFace;

    Frustum() = default;

    void create(glm::vec3 position, glm::vec3 direction, float nearPlane, float farPlane, float fov, float aspectRatio);
};

} //namespace lv

#endif
