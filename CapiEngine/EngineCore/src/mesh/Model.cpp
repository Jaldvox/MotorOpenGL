#include <mesh/Model.h>
#include <mesh/AssimpLoader.h>
#include <mesh/ModelMesh.h>
#include <surface/Material.h>
#include <core/Camera.h>
#include <utils/logger.h>

#include <algorithm>
#include <limits>

namespace cme {

    void Model::load(const std::string& path, const std::string& defaultShaderName) {
        _path      = path;
        _subMeshes = AssimpLoader::load(path, defaultShaderName);

        if (_subMeshes.empty()) {
            LOG_WARN("Model::load — ningún sub-mesh cargado para: " + path);
        }
        else {
            LOG_INFO(std::format("Model::load — {} sub-meshes cargados: {}", _subMeshes.size(), path));
        }
    }

    void Model::render(const glm::mat4& modelMatrix,
                       Camera*          cam,
                       ec::entity_t     ent) const
    {
        if (_subMeshes.empty()) return;

        for (const auto& sub : _subMeshes) {
            if (!sub.mesh || !sub.material) continue;

            ModelMesh* mesh = sub.mesh.get();

            // El material aplica su shader y sube luces, texturas y propiedades.
            sub.material->apply();

            // Subir matrices de cámara al shader que el material ya activó.
            Shader* shader = sub.material->getShader();

            cam->uploadProjectionToGPU(shader);

            mesh->setModelMatrix(modelMatrix);
            cam->uploadViewToGPU(shader, modelMatrix, mesh->normalMatrix(), ent);

            // Draw call — ModelMesh::render() no toca el shader, solo glDrawElements.
            mesh->render();
        }
    }

    void Model::getLocalAABB(glm::vec3& outMin, glm::vec3& outMax) const {
        outMin = glm::vec3( std::numeric_limits<float>::max());
        outMax = glm::vec3(-std::numeric_limits<float>::max());

        for (const auto& sub : _subMeshes) {
            if (!sub.mesh) continue;
            glm::vec3 sMin, sMax;
            sub.mesh->getLocalAABB(sMin, sMax);
            outMin = glm::min(outMin, sMin);
            outMax = glm::max(outMax, sMax);
        }
    }

} // namespace cme
