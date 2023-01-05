#ifndef LV_MESH_LOADER_H
#define LV_MESH_LOADER_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.hpp"

namespace lv {

class MeshLoader {
public:
	std::vector<MeshComponent> meshes;

	std::string directory;

#ifdef LV_BACKEND_VULKAN
	PipelineLayout* pipelineLayout;

	MeshLoader(PipelineLayout& aPipelineLayout) : pipelineLayout(&aPipelineLayout) {}
#endif

	void loadFromFile(const char* filename);

	void processNode(aiNode* node, const aiScene* scene);

	void processMesh(aiMesh* mesh, const aiScene* scene);

	Texture* loadMaterialTextures(aiMaterial* mat, aiTextureType type);
};

} //namespace lv

#endif
