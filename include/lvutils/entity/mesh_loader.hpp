#ifndef LV_MESH_LOADER_H
#define LV_MESH_LOADER_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.hpp"

namespace lv {

class MeshLoader {
public:
	uint8_t threadIndex = 0;

	std::vector<MeshComponent> meshes;

	std::string directory;

	PipelineLayout* pipelineLayout;

	MeshLoader(PipelineLayout& aPipelineLayout) : pipelineLayout(&aPipelineLayout) {}

	void loadFromFile(const char* filename);

	void processNode(aiNode* node, const aiScene* scene);

	void processMesh(aiNode* node, aiMesh* mesh, const aiScene* scene);

	Texture* loadMaterialTextures(aiMaterial* mat, aiTextureType type);
};

} //namespace lv

#endif
