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

    static VertexDescriptor* getVertexDescriptor();

    static VertexDescriptor* getVertexDescriptorShadows();
};

struct Vertex3D {
	//Attributes
	glm::vec3 position;

    static VertexDescriptor* getVertexDescriptor();
};

} //namespace lv

#endif
