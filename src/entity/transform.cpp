#include "lvutils/entity/transform.hpp"

namespace lv {

void TransformComponent::calcModel(glm::mat4 parentModel) {
    //Updating rotation to be within bounds
    if (rotation.x > 360.0f) rotation.x -= 360.0f;
    if (rotation.x < 0.0f) rotation.x += 360.0f;
    if (rotation.y > 360.0f) rotation.y -= 360.0f;
    if (rotation.y < 0.0f) rotation.y += 360.0f;
    if (rotation.z > 360.0f) rotation.z -= 360.0f;
    if (rotation.z < 0.0f) rotation.z += 360.0f;

    model.model = parentModel *
                  glm::translate(glm::mat4(1.0f), position) *
                  glm::toMat4(glm::quat(glm::radians(rotation))) *
                  glm::scale(glm::mat4(1.0f), scale);

    model.normalMatrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(model.model))));
}

} //namespace lv
