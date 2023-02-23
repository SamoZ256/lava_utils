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
		processMesh(node, mesh, scene);
	}
	// then do the same for each of its children
	for(unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

void MeshLoader::processMesh(aiNode* node, aiMesh* mesh, const aiScene* scene) {
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
	float x = node->mTransformation.a4;
	float y = node->mTransformation.b4;
	float z = node->mTransformation.c4;
	//std::cout << x << ", " << y << ", " << z << std::endl;

	for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
		MainVertex vertex;
		// process vertex positions, normals and texture coordinates
		vertex.position.x = x + mesh->mVertices[i].x;
		vertex.position.y = y + mesh->mVertices[i].y;
		vertex.position.z = z + mesh->mVertices[i].z;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		//vertex.tangent.w = 1.0f;//mesh->mTangents[i].w;
		//std::cout << bitangent1.x << ", " << bitangent1.y << ", " << bitangent1.z << std::endl;
		//std::cout << bitangent2.x << ", " << bitangent2.y << ", " << bitangent2.z << std::endl;
		//std::cout << "Tangent.w: " << vertex.tangent.w << " : " << glm::dot(bitangent1, bitangent2) << std::endl;

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

			glm::vec3 bitangent1 = glm::cross(vertex.normal, glm::vec3(vertex.tangent));
			glm::vec3 bitangent2 = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
			vertex.tangent.w = glm::dot(bitangent1, bitangent2) < 0.0f ? -1.0f : 1.0f;
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

		/*
		glm::vec3 pos1 = vertices[face.mIndices[0]].position;
		glm::vec3 pos2 = vertices[face.mIndices[1]].position;
		glm::vec3 pos3 = vertices[face.mIndices[2]].position;
		
		glm::vec2 uv1 = vertices[face.mIndices[0]].texCoord;
		glm::vec2 uv2 = vertices[face.mIndices[1]].texCoord;
		glm::vec2 uv3 = vertices[face.mIndices[02]].texCoord;

		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		vertices[face.mIndices[0]].tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		vertices[face.mIndices[0]].tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		vertices[face.mIndices[0]].tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		*/
	}

	// process material
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		Texture* texAlbedo = loadMaterialTextures(material, aiTextureType_DIFFUSE);
		Texture* texMetallicRoughness = loadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS);
		if (texMetallicRoughness == nullptr)
			texMetallicRoughness = loadMaterialTextures(material, aiTextureType_METALNESS);
		//Texture* texMetallic = loadMaterialTextures(material, aiTextureType_SPECULAR);
		Texture* texNormal = loadMaterialTextures(material, aiTextureType_HEIGHT);
		if (texNormal == nullptr)
			texNormal = loadMaterialTextures(material, aiTextureType_NORMALS);

		/*
		for (uint8_t i = 0; i < 22; i++) {
			std::cout << "Assimp texture" << (int)i << " found: " << material->GetTextureCount((aiTextureType)i) << std::endl;
		}
		*/

		textures.push_back(texAlbedo);
		textures.push_back(texMetallicRoughness);
		textures.push_back(texNormal);

		/*
		for (uint8_t i = 0; i < 3; i++) {
			std::cout << "Loaded texture " << (int)i << ": " << (textures[i] != nullptr) << std::endl;
		}
		*/

		//glm::vec3 center = glm::vec3((minX + maxX) / 2.0f, (minY + maxY) / 2.0f, (minZ + maxZ) / 2.0f);
		MeshComponent& newMesh = meshes.emplace_back(*pipelineLayout);
		newMesh.init(threadIndex, vertices, indices);
		for (uint8_t i = 0; i < textures.size(); i++) {
			//if (textures[i] != nullptr)
			newMesh.setTexture(textures[i], i);
		}

		newMesh.initDescriptorSet();
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

	return MeshComponent::loadTextureFromFile(threadIndex, filename.c_str(), (type == aiTextureType_DIFFUSE ? LV_FORMAT_R8G8B8A8_UNORM_SRGB : LV_FORMAT_R8G8B8A8_UNORM));
}

} //namespace lv
