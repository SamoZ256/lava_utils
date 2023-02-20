#include "lvutils/entity/vertex.hpp"

#include "lvcore/core/enums.hpp"

namespace lv {

//Main vertex
VertexDescriptor* MainVertex::getVertexDescriptor() {
	lv::VertexDescriptor* vertexDesc = new lv::VertexDescriptor(
#ifdef LV_BACKEND_VULKAN
		0
#elif defined LV_BACKEND_METAL
		BINDING_INDEX
#endif
	, sizeof(MainVertex)
	);

	//Position
	vertexDesc->addBinding(0, LV_VERTEX_FORMAT_RGB32_SFLOAT, offsetof(MainVertex, position));

	//Texture coordinates
	vertexDesc->addBinding(1, LV_VERTEX_FORMAT_RG32_SFLOAT, offsetof(MainVertex, texCoord));

	//Normal
	vertexDesc->addBinding(2, LV_VERTEX_FORMAT_RGB32_SFLOAT, offsetof(MainVertex, normal));

	//Tangent
	vertexDesc->addBinding(3, LV_VERTEX_FORMAT_RGBA32_SFLOAT, offsetof(MainVertex, tangent));

	//Bitangent
	//vertexDesc->addBinding(4, LV_VERTEX_FORMAT_RGB32_SFLOAT, offsetof(MainVertex, bitangent));

	//Bitangent
	/*
	attributeDescriptions[4].binding = 0;
	attributeDescriptions[4].location = 4;
	attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[4].offset = offsetof(MainVertex, bitangent);
	*/

	return vertexDesc;
}

VertexDescriptor* MainVertex::getVertexDescriptorShadows() {
	lv::VertexDescriptor* vertexDesc = new lv::VertexDescriptor(
#ifdef LV_BACKEND_VULKAN
		0
#elif defined LV_BACKEND_METAL
		BINDING_INDEX_SHADOWS
#endif
	, sizeof(MainVertex)
	);

	//Position
	vertexDesc->addBinding(0, LV_VERTEX_FORMAT_RGB32_SFLOAT, offsetof(MainVertex, position));

	return vertexDesc;
}

//3D vertex
VertexDescriptor* Vertex3D::getVertexDescriptor() {
	lv::VertexDescriptor* vertexDesc = new lv::VertexDescriptor(
#ifdef LV_BACKEND_VULKAN
		0
#elif defined LV_BACKEND_METAL
		BINDING_INDEX
#endif
	, sizeof(Vertex3D)
	);

	//Position
	vertexDesc->addBinding(0, LV_VERTEX_FORMAT_RGB32_SFLOAT, offsetof(Vertex3D, position));

	return vertexDesc;
}

} //namespace lv
