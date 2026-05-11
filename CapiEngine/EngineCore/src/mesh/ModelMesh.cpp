#include <mesh/ModelMesh.h>
#include <mesh/MeshID.h>

namespace cme {

    ModelMesh::ModelMesh(const std::vector<glm::vec3>& vertices,
                         const std::vector<glm::vec3>& normals,
                         const std::vector<glm::vec2>& texCoords,
                         const std::vector<glm::uvec3>& indices)
    {
        _vertices  = vertices;
        _normals   = normals;
        _texCoords = texCoords;
        _indices   = indices;
        _id        = MODEL;

        generateMesh();
    }

    void ModelMesh::generateMesh() {
        // Los datos ya están en los vectores del padre.
        // Simplemente subimos los buffers a la GPU.
        if (!_vertices.empty())
            initBuffers();
    }

    void ModelMesh::render() const {
        // El shader ya está activo: Material::apply() lo activó antes de llegar aquí.
        // Solo hacemos el draw call.
        glBindVertexArray(_VAO);

        if (!_indices.empty())
            glDrawElements(mPrimitive, static_cast<GLsizei>(_indices.size() * 3), GL_UNSIGNED_INT, nullptr);
        else
            glDrawArrays(mPrimitive, 0, static_cast<GLsizei>(_vertices.size()));

        glBindVertexArray(0);
    }

}