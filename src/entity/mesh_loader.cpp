#include "lvutils/entity/mesh_loader.hpp"

#include <iostream>

namespace lv {

void MeshLoader::loadFromFile(const char* filename) {
	std::cout << "Loading model '" << filename << "'" << std::endl;
	//std::cout << "Flip: " << flipUVs << std::endl;
	Assimp::Importer import;
	//filename = getRelativeToAssetsPath(filename);

	unsigned int flags = aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_FlipUVs;
	std::string strFilename(filename);
	const aiScene *scene = import.ReadFile(strFilename, flags);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)  {
		std::cout << "Error: Could not load model '" << filename << "' ->\n" << import.GetErrorString() << std::endl;
		return;
	}
	directory = strFilename.substr(0, strFilename.find_last_of("/"));

	processNode(scene->mRootNode, scene);
}

void MeshLoader::processNode(aiNode* node, const aiScene* scene) {
	// process all the node's meshes (if any)
	for(unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, scene);
	}
	// then do the same for each of its children
	for(unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

void MeshLoader::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<MainVertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture*> textures;

	/*
	float maxX = std::numeric_limits<float>::min();
	float maxY = std::numeric_limits<float>::min();
	float maxZ = std::numeric_limits<float>::min();
	float minX = std::numeric_limits<float>::max();
	float minY = std::numeric_limits<float>::max();
	float minZ = std::numeric_limits<float>::max();
	*/

	for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
		MainVertex vertex;
		// process vertex positions, normals and texture coordinates
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		/*
		vertex.bitangent.x = mesh->mBitangents[i].x;
		vertex.bitangent.y = mesh->mBitangents[i].y;
		vertex.bitangent.z = mesh->mBitangents[i].z;
		*/

		if (mesh->mTextureCoords[0]) {
			vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = mesh->mTextureCoords[0][i].y;

			vertex.tangent.x = mesh->mTangents[i].x;
			vertex.tangent.y = mesh->mTangents[i].y;
			vertex.tangent.z = mesh->mTangents[i].z;
		} else {
			vertex.texCoord = glm::vec2(0.0f);
		}
		//vertex.texCoord = glm::vec2(0.8f);

		vertices.push_back(vertex);
	}

	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	// process material
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		Texture* texAlbedo = loadMaterialTextures(material, aiTextureType_DIFFUSE);
		Texture* texMetallicRoughness = loadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS);
		if (texMetallicRoughness == nullptr)
			texMetallicRoughness = loadMaterialTextures(material, aiTextureType_METALNESS);
		//Texture* texMetallic = loadMaterialTextures(material, aiTextureType_SPECULAR);
		//Texture* texNormal = loadMaterialTextures(material, aiTextureType_HEIGHT);

		/*
		for (uint8_t i = 0; i < 22; i++) {
			std::cout << "Assimp texture" << (int)i << " found: " << material->GetTextureCount((aiTextureType)i) << std::endl;
		}
		*/

//#ifdef LV_BACKEND_VULKAN
		textures.push_back(texAlbedo);
		textures.push_back(texMetallicRoughness);
		//textures.push_back(texMetallic);
		//textures.push_back(texNormal);
		/*
#elif defined LV_BACKEND_METAL
		textures.push_back(texRoughness);
		textures.push_back(texAlbedo);
		textures.push_back(texMetallic);
#endif
		*/

		/*
		for (uint8_t i = 0; i < 3; i++) {
			std::cout << "Loaded texture " << (int)i << ": " << (textures[i] != nullptr) << std::endl;
		}
		*/

		//glm::vec3 center = glm::vec3((minX + maxX) / 2.0f, (minY + maxY) / 2.0f, (minZ + maxZ) / 2.0f);
		MeshComponent& newMesh = meshes.emplace_back(
#ifdef LV_BACKEND_VULKAN
			*pipelineLayout
#endif
		);
		newMesh.init(vertices, indices);
		for (uint8_t i = 0; i < textures.size(); i++) {
			//if (textures[i] != nullptr)
			newMesh.setTexture(textures[i], i);
		}

#ifdef LV_BACKEND_VULKAN
		newMesh.initDescriptorSet();
#endif
		/*
		std::cout << "Init: " << aiTextureType_DIFFUSE << " : " << aiTextureType_SPECULAR << " : " << aiTextureType_HEIGHT << " : " << aiTextureType_SHININESS << " : " << aiTextureType_AMBIENT << std::endl;
		for (int i = 0; i <= aiTextureType_UNKNOWN; i++) {
		int num = material->GetTextureCount(static_cast<aiTextureType>(i));
		if (num > 0) {
			std::cout << i << " -> " << num << std::endl;
		}
		}
		*/
	}
}

Texture* MeshLoader::loadMaterialTextures(aiMaterial *mat, aiTextureType type) {
	aiString str;
	int count = mat->GetTextureCount(type);

	//for (int i = 0; i < count; i++) {
	mat->GetTexture(type, 0, &str);
	std::string texStr = std::string(str.C_Str());
	std::string filename = directory + "/" + texStr;
	//std::cout << str.C_Str() << " : " << currentDir << std::endl;
	if (texStr == "")
		return nullptr;

	return MeshComponent::loadTextureFromFile(filename.c_str());
}

} //namespace lv
