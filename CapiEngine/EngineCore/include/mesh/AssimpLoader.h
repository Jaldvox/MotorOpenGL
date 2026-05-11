#pragma once
#include <string>
#include <vector>
#include <memory>
#include <surface/Material.h>
#include <surface/Texture.h>
#include <mesh/ModelMesh.h>

struct aiNode;
struct aiMesh;
struct aiMaterial;
struct aiScene;
enum aiTextureType;

namespace cme {

    struct SubMesh {
        std::unique_ptr<ModelMesh> mesh;
        std::unique_ptr<Material> material;
        std::vector<std::unique_ptr<Texture>> ownedTextures;
        glm::mat4 localTransform = glm::mat4(1.0f);
    };

    class AssimpLoader {
    public:
        static std::vector<SubMesh> load(const std::string& path,
                                         const std::string& defaultShaderName = "default");
    private:
        static void processNode(const aiNode*        node,
                                const aiScene*       scene,
                                const std::string&   directory,
                                const std::string&   defaultShaderName,
                                std::vector<SubMesh>& out,
                                const glm::mat4&     parentTransform);

        static SubMesh processMesh(const aiMesh* mesh, const aiScene* scene, const std::string& directory, const std::string& defaultShaderName);

        static Texture* loadMaterialTexture(const aiMaterial*  mat,
                                            aiTextureType      type,
                                            const std::string& directory,
                                            SubMesh&           outSubMesh);

        // Prueba varios tipos de textura en orden y devuelve el primero que tenga datos.
        // Necesario porque FBX PBR usa BASE_COLOR en vez de DIFFUSE, etc.
        static aiTextureType resolveType(const aiMaterial* mat, std::initializer_list<aiTextureType> candidates);
    };

} // namespace cme
