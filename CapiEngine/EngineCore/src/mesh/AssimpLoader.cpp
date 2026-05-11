#include <mesh/AssimpLoader.h>
#include <mesh/ModelMesh.h>
#include <managers/ResourceManager.h>
#include <utils/logger.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace cme {

	// load
	std::vector<SubMesh> AssimpLoader::load(const std::string& path,
		const std::string& defaultShaderName)
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(path,
			aiProcess_Triangulate |  // todo a triángulos
			aiProcess_FlipUVs |  // OpenGL: UV origen abajo-izquierda
			aiProcess_GenSmoothNormals |  // genera normales si el modelo no las tiene
			aiProcess_CalcTangentSpace |  // útil para normal mapping futuro
			aiProcess_JoinIdenticalVertices |
			aiProcess_PreTransformVertices);

		if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
			LOG_ERROR(std::string("AssimpLoader::load — ") + importer.GetErrorString());
			return {};
		}

		// Directorio base del archivo (para resolver texturas relativas)
		std::string directory = path.substr(0, path.find_last_of("/\\"));

		std::vector<SubMesh> result;
		processNode(scene->mRootNode, scene, directory, defaultShaderName, result);
		return result;
	}

	// processNode (recursivo)
	void AssimpLoader::processNode(const aiNode* node,
		const aiScene* scene,
		const std::string& directory,
		const std::string& defaultShaderName,
		std::vector<SubMesh>& out)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
			const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			out.push_back(processMesh(mesh, scene, directory, defaultShaderName));
		}

		for (unsigned int i = 0; i < node->mNumChildren; ++i)
			processNode(node->mChildren[i], scene, directory, defaultShaderName, out);
	}

	// processMesh
	SubMesh AssimpLoader::processMesh(const aiMesh* mesh,
		const aiScene* scene,
		const std::string& directory,
		const std::string& defaultShaderName)
	{
		std::vector<glm::vec3>  vertices;
		std::vector<glm::vec3>  normals;
		std::vector<glm::vec2>  texCoords;
		std::vector<glm::uvec3> indices;

		vertices.reserve(mesh->mNumVertices);
		normals.reserve(mesh->mNumVertices);
		texCoords.reserve(mesh->mNumVertices);

		// --- Vértices ---
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			vertices.emplace_back(mesh->mVertices[i].x,
				mesh->mVertices[i].y,
				mesh->mVertices[i].z);

			if (mesh->HasNormals())
				normals.emplace_back(mesh->mNormals[i].x,
					mesh->mNormals[i].y,
					mesh->mNormals[i].z);
			else
				normals.emplace_back(0.0f, 1.0f, 0.0f); // normal por defecto: arriba

			// Assimp soporta hasta 8 canales UV; usamos el primero
			if (mesh->mTextureCoords[0])
				texCoords.emplace_back(mesh->mTextureCoords[0][i].x,
					mesh->mTextureCoords[0][i].y);
			else
				texCoords.emplace_back(0.0f, 0.0f);
		}

		// --- Índices ---
		indices.reserve(mesh->mNumFaces);
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			const aiFace& face = mesh->mFaces[i];
			// Tras aiProcess_Triangulate siempre son 3 índices
			if (face.mNumIndices == 3)
				indices.emplace_back(face.mIndices[0],
					face.mIndices[1],
					face.mIndices[2]);
		}

		// --- Construir SubMesh ---
		SubMesh sub;
		sub.mesh = std::make_unique<ModelMesh>(vertices, normals, texCoords, indices);

		// --- Material ---
		sub.material = std::make_unique<Material>();

		Shader* shader = rscrM().getShader(defaultShaderName);
		if (shader)
			sub.material->setShader(shader);
		else
			LOG_WARN("AssimpLoader: shader '" + defaultShaderName + "' no encontrado en ResourceManager");

		if (mesh->mMaterialIndex >= 0) {
			const aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];

			// Color difuso como fallback si no hay textura
			aiColor3D color(1.0f, 1.0f, 1.0f);
			aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			sub.material->setVec3("albedoColor", glm::vec3(color.r, color.g, color.b));

			// Textura difusa / albedo
			Texture* diffuseTex = loadMaterialTexture(aiMat, aiTextureType_DIFFUSE, directory, sub);
			Texture* specTex = loadMaterialTexture(aiMat, aiTextureType_SPECULAR, directory, sub);
			Texture* normalTex = loadMaterialTexture(aiMat, aiTextureType_NORMALS, directory, sub);
			Texture* roughTex = loadMaterialTexture(aiMat, aiTextureType_DIFFUSE_ROUGHNESS, directory, sub);

			// Los nombres de uniform deben coincidir con tu shader
			if (diffuseTex) sub.material->setTexture("albedoMap", diffuseTex);
			if (specTex)    sub.material->setTexture("specularMap", specTex);
			if (normalTex)  sub.material->setTexture("normalMap", normalTex);
			if (roughTex)   sub.material->setTexture("roughnessMap", roughTex);
		}

		return sub;
	}

	// loadMaterialTexture
	Texture* AssimpLoader::loadMaterialTexture(const aiMaterial* mat,
		aiTextureType     type,
		const std::string& directory,
		SubMesh& outSubMesh)
	{
		LOG_INFO(std::format("AssimpLoader: buscando textura en: {}", directory));
		if (mat->GetTextureCount(type) == 0)
			return nullptr;

		aiString aiPath;
		mat->GetTexture(type, 0, &aiPath);   // solo la primera del tipo
		std::string relPath(aiPath.C_Str());

		// Normalizar separadores
		std::replace(relPath.begin(), relPath.end(), '\\', '/');

		std::string fullPath = directory + "/" + relPath;
		std::string texName = std::filesystem::path(relPath).stem().string();

		// ¿Ya está cargada en ResourceManager? La reutilizamos (sin ownership aquí)
		Texture* existing = rscrM().getTexture(texName);
		if (existing)
			return existing;

		// Si no, la cargamos y la guardamos en el SubMesh (owner) 
		auto tex = std::make_unique<Texture>();
		TextureType texType = (type == aiTextureType_DIFFUSE)
			? TextureType::DEFAULT   // sRGB para albedo
			: TextureType::LINEAR;   // lineal para normal/specular/rough

		tex->load(fullPath, texType);
		tex->setName(texName);

		Texture* rawPtr = tex.get();
		outSubMesh.ownedTextures.push_back(std::move(tex));

		// Registramos en ResourceManager para que sea reutilizable en otros modelos
		rscrM().registerTexture(texName, rawPtr);

		return rawPtr;
	}

}
