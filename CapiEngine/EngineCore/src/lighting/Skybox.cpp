#include <lighting/Skybox.h>
#include <core/Camera.h>
#include <utils/logger.h>
#include <glm/gtc/matrix_transform.hpp>
#include <managers/ResourceManager.h>
namespace cme {
    extern const float cubeVertices[108];

    Skybox::~Skybox() {
        if (_VAO) { glDeleteVertexArrays(1, &_VAO); glDeleteBuffers(1, &_VBO); }
    }

    void Skybox::initGeometry() {
        if (_VAO) return;
        glGenVertexArrays(1, &_VAO);
        glGenBuffers(1, &_VBO);
        glBindVertexArray(_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }

    bool Skybox::load(Shader* shader) {
        std::array<GLuint, 6> ids;
        std::array<int, 6>    widths, heights;

        for (int i = 0; i < 6; i++) {
            Texture* tex = rscrM().getTexture(_faceNames[i]);
            if (!tex) {
                LOG_ERROR("Textura no encontrada: " + _faceNames[i]);
                return false;
            }
            ids[i] = tex->id();
            widths[i] = tex->width();
            heights[i] = tex->height();
        }

        _cubemap.loadCubemap(ids, widths, heights);
        _shader = shader;
        _loaded = true;
        initGeometry();
        return true;
    }

    void Skybox::render(Camera* cam) {
        if (!_loaded || !_shader) return;

        glDepthFunc(GL_LEQUAL);
        _shader->use();

        glDisable(GL_CULL_FACE);

        // Quitar traslación de la view para que el skybox no se mueva
        glm::mat4 view = glm::mat4(glm::mat3(cam->getViewMat()));
        _shader->setUniform("view", view);
        _shader->setUniform("projection", cam->getProjectionMat());
        _shader->setUniform("skybox", 0);

        _cubemap.bind();
        glBindVertexArray(_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glEnable(GL_CULL_FACE);

        glDepthFunc(GL_LESS);
    }

    void Skybox::serialize(JsonSerializer& s) const {
        for (int i = 0; i < 6; i++)
            s.write(FACE_NAMES[i], _faceNames[i]);
    }

    void Skybox::deserialize(JsonSerializer& s) {
        for (int i = 0; i < 6; i++) {
            _faceNames[i] = s.readString(FACE_NAMES[i]);
        }

        load(rscrM().getShader("skybox"));
    }

    const float cubeVertices[108] = {
        // posiciones          
        // Cara trasera
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        // Cara izquierda
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        // Cara derecha
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

         // Cara frontal
         -1.0f, -1.0f,  1.0f,
         -1.0f,  1.0f,  1.0f,
          1.0f,  1.0f,  1.0f,
          1.0f,  1.0f,  1.0f,
          1.0f, -1.0f,  1.0f,
         -1.0f, -1.0f,  1.0f,

         // Cara superior
         -1.0f,  1.0f, -1.0f,
          1.0f,  1.0f, -1.0f,
          1.0f,  1.0f,  1.0f,
          1.0f,  1.0f,  1.0f,
         -1.0f,  1.0f,  1.0f,
         -1.0f,  1.0f, -1.0f,

         // Cara inferior
         -1.0f, -1.0f, -1.0f,
         -1.0f, -1.0f,  1.0f,
          1.0f, -1.0f, -1.0f,
          1.0f, -1.0f, -1.0f,
         -1.0f, -1.0f,  1.0f,
          1.0f, -1.0f,  1.0f
    };
}