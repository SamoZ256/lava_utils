#ifndef LV_VERTEX_H
#define LV_VERTEX_H

#include <vector>

#include "lvcore/core/vertex_descriptor.hpp"

#include "../libraries/glm.hpp"

namespace lv {

struct MainVertex {
    //Attributes
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
    glm::vec4 tangent;
    //glm::vec3 bitangent;

#ifdef LV_BACKEND_METAL
    static constexpr uint8_t BINDING_INDEX = 2;
    static constexpr uint8_t BINDING_INDEX_SHADOWS = 2;
#endif

    static VertexDescriptor* getVertexDescriptor();

    static VertexDescriptor* getVertexDescriptorShadows();
};

struct Vertex3D {
	//Attributes
	glm::vec3 position;

#ifdef LV_BACKEND_METAL
    static constexpr uint8_t BINDING_INDEX = 1;
#endif

    static VertexDescriptor* getVertexDescriptor();
};

} //namespace lv

#endif
