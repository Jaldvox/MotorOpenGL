#include <mesh/Model.h>
#include <mesh/AssimpLoader.h>
#include <mesh/ModelMesh.h>
#include <surface/Material.h>
#include <core/Camera.h>
#include <utils/logger.h>

#include <limits>

namespace cme {

    void Model::load(const std::string& path, const std::string& defaultShaderName) {
        _path      = path;
        _subMeshes = AssimpLoader::load(path, defaultShaderName);

        if (_subMeshes.empty()) {
            LOG_WARN("Model::load — ningún sub-mesh cargado para: " + path);
        }
        else {
            LOG_INFO(std::format("Model::load — {} sub-meshes: {}", _subMeshes.size(), path));
        }
    }

    void Model::render(const glm::mat4& modelMatrix, Camera* cam, ec::entity_t ent) const {
        for (const auto& sub : _subMeshes) {
            if (!sub.mesh || !sub.material) continue;

            // Transform final = transform del entity × transform del nodo Assimp
            glm::mat4 finalModel = modelMatrix * sub.localTransform;

            sub.material->apply();

            Shader* shader = sub.material->getShader();
            if (!shader) continue;

            cam->uploadProjectionToGPU(shader);

            sub.mesh->setModelMatrix(finalModel);
            cam->uploadViewToGPU(shader, finalModel, sub.mesh->normalMatrix(), ent);

            sub.mesh->render();
        }
    }

    void Model::renderDepth(Shader* depthShader) const {
        for (const auto& sub : _subMeshes) {
            if (!sub.mesh || !sub.material) continue;

            sub.mesh->renderDepth(depthShader);
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

} 
