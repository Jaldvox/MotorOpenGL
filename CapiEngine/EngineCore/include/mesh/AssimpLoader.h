#pragma once
#include <string>
#include <vector>
#include <memory>
#include <surface/Material.h>
#include <surface/Texture.h>
#include <mesh/ModelMesh.h>

// Forward-declarations de Assimp para no contaminar headers de usuario
struct aiNode;
struct aiMesh;
struct aiMaterial;
struct aiScene;
enum aiTextureType;

namespace cme {

    /// @brief Un sub-mesh con su material asociado.
    ///        Model es propietario de estos structs.
    struct SubMesh {
        std::unique_ptr<ModelMesh> mesh;
        std::unique_ptr<Material> material;
        /// Texturas creadas para este sub-mesh. El Material apunta a ellas (sin ownership).
        std::vector<std::unique_ptr<Texture>> ownedTextures;
        glm::mat4 localTransform = glm::mat4(1.0f);
    };

    /// @brief Carga un archivo 3D con Assimp y devuelve sus sub-meshes listos para renderizar.
    ///        Compatible con el sistema Material/Texture/Shader existente.
    class AssimpLoader {
    public:
        /// @brief Carga el archivo en `path` y devuelve todos sus sub-meshes.
        /// @param path Ruta absoluta o relativa al archivo (.obj, .fbx, .gltf, ...)
        /// @param defaultShaderName Nombre del shader del ResourceManager que se asignará
        ///        a los materiales (p. ej. "default" o "phong").
        static std::vector<SubMesh> load(const std::string& path, const std::string& defaultShaderName = "default");

    private:
        static void processNode(const aiNode*    node,
                                const aiScene*   scene,
                                const std::string& directory,
                                const std::string& defaultShaderName,
                                std::vector<SubMesh>& out);

        static SubMesh processMesh(const aiMesh*     mesh,
                                   const aiScene*    scene,
                                   const std::string& directory,
                                   const std::string& defaultShaderName);

        /// @brief Carga una textura Assimp al sistema Texture existente.
        ///        Si ya existe en ResourceManager la reutiliza; si no, la crea y la registra.
        /// @return Par (textura, bool señalando si el caller debe tomar ownership)
        static Texture* loadMaterialTexture(const aiMaterial* mat, aiTextureType type, const std::string& directory, SubMesh& outSubMesh);
    };

} // namespace cme
