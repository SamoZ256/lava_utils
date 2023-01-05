#ifndef LV_TRANSFORM_H
#define LV_TRANSFORM_H

#include "../libraries/glm.hpp"

//#include "../Core/Renderer.hpp"

namespace lv {

struct PushConstantModel {
  glm::mat4 model;
  glm::mat4 normalMatrix;
};

class TransformComponent {
public:
  PushConstantModel model;

  glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

  void calcModel(glm::mat4 parentModel);

  //void uploadModel(VkPipelineLayout& pipelineLayout);
};

} //namespace lv

#endif
