#pragma once
#include <mesh/Mesh.h>

namespace cme {

    /// @brief Mesh que se rellena con datos cargados externamente (ej: Assimp).
    ///        Los datos se pasan en el constructor; generateMesh() solo sube los buffers.
    class ModelMesh : public Mesh {
    public:
        ModelMesh() = default;
        ~ModelMesh() = default;

        /// @brief Construye el mesh con los datos ya procesados por el loader.
        ModelMesh(const std::vector<glm::vec3>& vertices,
                  const std::vector<glm::vec3>& normals,
                  const std::vector<glm::vec2>& texCoords,
                  const std::vector<glm::uvec3>& indices);

        /// @brief Con ModelMesh los datos ya vienen del constructor, esto sube los VBOs.
        void generateMesh() override;

        /// @brief Solo ejecuta el draw call. El shader ya fue activado por Material::apply()
        ///        antes de llamar a este método, así que no tocamos _shader aquí.
        void render() const override;
    };

} // namespace cme
