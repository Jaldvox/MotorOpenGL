#pragma once
#include <string>
#include <vector>
#include <mesh/AssimpLoader.h> 
#include <glm/mat4x4.hpp>
#include <ec/entity.h>

namespace cme {

    class Camera;

    /// @brief Contenedor de un modelo 3D cargado con Assimp.
    ///        Un modelo tiene N sub-meshes, cada uno con su propio Material.
    class Model {
    public:
        Model(std::string name) : _name(name) {}

        ~Model() = default;
        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;
        Model(Model&&) = default;
        Model& operator=(Model&&) = default;

        /// @brief Carga el archivo en `path` usando AssimpLoader.
        /// @param defaultShaderName Nombre del shader por defecto en ResourceManager.
        void load(const std::string& path,
                  const std::string& defaultShaderName = "default");

        /// @brief Renderiza todos los sub-meshes.
        /// Llama a material->apply(), sube matrices de cámara y dibuja la geometría.
        /// @param modelMatrix  Matriz de modelado del transform de la entidad.
        /// @param cam Cámara activa (para subir matrices de proyecto/vista).
        /// @param ent Entidad dueña (necesaria para Camera::uploadViewToGPU).
        void render(const glm::mat4& modelMatrix,
                    Camera*          cam,
                    ec::entity_t     ent) const;

        void renderDepth(Shader* depthShader) const;

        bool isLoaded() const { return !_subMeshes.empty(); }
        const std::string& path() const { return _path; }
        const std::string& name() const { return _name; }

        std::vector<SubMesh>&       subMeshes()       { return _subMeshes; }
        const std::vector<SubMesh>& subMeshes() const { return _subMeshes; }

        void getLocalAABB(glm::vec3& outMin, glm::vec3& outMax) const;

    private:
        std::vector<SubMesh> _subMeshes;
        std::string _path;
        std::string _name;
    };

} 
