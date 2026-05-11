#include <mesh/AssimpLoader.h>
#include <mesh/ModelMesh.h>
#include <managers/ResourceManager.h>
#include <utils/logger.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>

namespace cme {

	aiTextureType AssimpLoader::resolveType(const aiMaterial* mat,std::initializer_list<aiTextureType> candidates) {
		for (auto t : candidates)
			if (mat->GetTextureCount(t) > 0) return t;
		return aiTextureType_NONE;
	}

	std::vector<SubMesh> AssimpLoader::load(const std::string& path, const std::string& defaultShaderName) {
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(path,
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_JoinIdenticalVertices);
		// SIN aiProcess_PreTransformVertices — manejamos las transforms manualmente

		if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
			LOG_ERROR(std::string("AssimpLoader::load — ") + importer.GetErrorString());
			return {};
		}

		std::string directory = path.substr(0, path.find_last_of("/\\"));

		std::vector<SubMesh> result;
		processNode(scene->mRootNode, scene, directory, defaultShaderName,
			result, glm::mat4(1.0f));
		return result;
	}

	void AssimpLoader::processNode(const aiNode* node, const aiScene* scene, const std::string& directory, const std::string& defaultShaderName,
		std::vector<SubMesh>& out, const glm::mat4& parentTransform) {

		// Assimp guarda matrices en row-major; GLM las espera column-major → transponemos
		glm::mat4 nodeLocal = glm::transpose(
			glm::make_mat4(&node->mTransformation.a1));

		glm::mat4 globalTransform = parentTransform * nodeLocal;

		for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
			const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			SubMesh sub = processMesh(mesh, scene, directory, defaultShaderName);
			sub.localTransform = globalTransform;
			out.push_back(std::move(sub));
		}

		for (unsigned int i = 0; i < node->mNumChildren; ++i)
			processNode(node->mChildren[i], scene, directory, defaultShaderName,
				out, globalTransform);
	}

	SubMesh AssimpLoader::processMesh(const aiMesh* mesh, const aiScene* scene, const std::string& directory, const std::string& defaultShaderName) {
		std::vector<glm::vec3>  vertices;
		std::vector<glm::vec3>  normals;
		std::vector<glm::vec2>  texCoords;
		std::vector<glm::uvec3> indices;

		vertices.reserve(mesh->mNumVertices);
		normals.reserve(mesh->mNumVertices);
		texCoords.reserve(mesh->mNumVertices);

		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			vertices.emplace_back(mesh->mVertices[i].x,
				mesh->mVertices[i].y,
				mesh->mVertices[i].z);

			normals.emplace_back(mesh->HasNormals()
				? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
				: glm::vec3(0.0f, 1.0f, 0.0f));

			texCoords.emplace_back(mesh->mTextureCoords[0]
				? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
				: glm::vec2(0.0f));
		}

		indices.reserve(mesh->mNumFaces);
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			const aiFace& face = mesh->mFaces[i];
			if (face.mNumIndices == 3)
				indices.emplace_back(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
		}

		SubMesh sub;
		sub.mesh = std::make_unique<ModelMesh>(vertices, normals, texCoords, indices);
		sub.material = std::make_unique<Material>();

		Shader* shader = rscrM().getShader(defaultShaderName);
		if (shader)
			sub.material->setShader(shader);
		else
			LOG_WARN("AssimpLoader: shader '" + defaultShaderName + "' no encontrado");

		if (mesh->mMaterialIndex >= 0) {
			const aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];

			// Color difuso fallback
			aiColor3D color(1.0f, 1.0f, 1.0f);
			aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			sub.material->setVec3("albedoColor", glm::vec3(color.r, color.g, color.b));

			// Albedo: BASE_COLOR (FBX PBR) o DIFFUSE (legacy OBJ/FBX)
			auto diffuseType = resolveType(aiMat, { aiTextureType_BASE_COLOR,
													 aiTextureType_DIFFUSE });
			// Normal: NORMAL_CAMERA (FBX PBR) o NORMALS (legacy)
			auto normalType = resolveType(aiMat, { aiTextureType_NORMAL_CAMERA,
													 aiTextureType_NORMALS });
			// Metalness
			auto metalType = resolveType(aiMat, { aiTextureType_METALNESS,
													 aiTextureType_SPECULAR });
			// Roughness
			auto roughType = resolveType(aiMat, { aiTextureType_DIFFUSE_ROUGHNESS,
													 aiTextureType_SHININESS });

			Texture* diffuseTex = (diffuseType != aiTextureType_NONE)
				? loadMaterialTexture(aiMat, diffuseType, directory, sub) : nullptr;
			Texture* normalTex = (normalType != aiTextureType_NONE)
				? loadMaterialTexture(aiMat, normalType, directory, sub) : nullptr;
			Texture* metalTex = (metalType != aiTextureType_NONE)
				? loadMaterialTexture(aiMat, metalType, directory, sub) : nullptr;
			Texture* roughTex = (roughType != aiTextureType_NONE)
				? loadMaterialTexture(aiMat, roughType, directory, sub) : nullptr;

			// Nombres de uniform — ajusta si tu shader usa otros
			if (diffuseTex) sub.material->setTexture("albedoMap", diffuseTex);
			if (normalTex)  sub.material->setTexture("normalMap", normalTex);
			if (metalTex)   sub.material->setTexture("metallicMap", metalTex);
			if (roughTex)   sub.material->setTexture("roughnessMap", roughTex);
		}

		if (mesh->mMaterialIndex >= 0) {
			const aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
			aiString matName;
			aiMat->Get(AI_MATKEY_NAME, matName);
			LOG_INFO("=== Material: " + std::string(matName.C_Str()) + " ===");

			// Nombres de todos los tipos conocidos de Assimp
			const char* typeNames[] = {
				"NONE","DIFFUSE","SPECULAR","AMBIENT","EMISSIVE","HEIGHT",
				"NORMALS","SHININESS","OPACITY","DISPLACEMENT","LIGHTMAP",
				"REFLECTION","BASE_COLOR","NORMAL_CAMERA","EMISSION_COLOR",
				"METALNESS","DIFFUSE_ROUGHNESS","AMBIENT_OCCLUSION","UNKNOWN"
			};
			for (int t = 1; t <= 18; ++t) {
				unsigned int count = aiMat->GetTextureCount((aiTextureType)t);
				if (count > 0) {
					aiString p;
					aiMat->GetTexture((aiTextureType)t, 0, &p);
					LOG_INFO(std::format("  tipo {:2d} ({:20s}) → {}",
						t, typeNames[t], p.C_Str()));
				}
			}
		}

		return sub;
	}

	Texture* AssimpLoader::loadMaterialTexture(const aiMaterial* mat, aiTextureType type, const std::string& directory, SubMesh& outSubMesh) {
		aiString aiPath;
		mat->GetTexture(type, 0, &aiPath);
		std::string relPath(aiPath.C_Str());
		std::replace(relPath.begin(), relPath.end(), '\\', '/');

		std::string filename = std::filesystem::path(relPath).filename().string();
		std::string texName = std::filesystem::path(relPath).stem().string();

		// Caché primero — ResourceManager ya la puede tener cargada
		// (ProjectLoader carga texturas sueltas de assets/ antes o después del modelo)
		Texture* existing = rscrM().getTexture(texName);
		if (existing) {
			LOG_INFO("AssimpLoader: textura reutilizada desde ResourceManager: " + texName);
			return existing;
		}

		// Buscar en disco con varias estrategias de path
		std::vector<std::string> candidates = {
			directory + "/" + relPath,               // path relativo tal cual
			directory + "/" + filename,              // solo nombre en carpeta del modelo
			directory + "/textures/" + filename,     // subcarpeta textures/
			directory + "/Textures/" + filename,     // ídem con mayúscula
			directory + "/maps/" + filename,
		};

		std::string fullPath;
		for (auto& c : candidates) {
			if (std::filesystem::exists(c)) {
				fullPath = c;
				break;
			}
		}

		if (fullPath.empty()) {
			LOG_WARN("AssimpLoader: textura no encontrada en disco: " + filename
				+ "  (directorio modelo: " + directory + ")");
			return nullptr;
		}

		// sRGB solo para albedo/diffuse; lineal para todo lo demás
		TextureType texType = (type == aiTextureType_DIFFUSE ||
			type == aiTextureType_BASE_COLOR)
			? TextureType::DEFAULT
			: TextureType::LINEAR;

		auto tex = std::make_unique<Texture>();
		tex->load(fullPath, texType);
		tex->setName(texName);
		LOG_INFO("AssimpLoader: textura cargada: " + fullPath);

		Texture* rawPtr = tex.get();
		outSubMesh.ownedTextures.push_back(std::move(tex));
		rscrM().registerTexture(texName, rawPtr);

		return rawPtr;
	}

}
