#pragma once
#include <surface/Texture.h>
#include <surface/Shader.h>
#include <array>
#include <string>
#include <serialize/Serializable.h>

namespace cme {
    class Camera;

    class Skybox : public Serializable {
    private:
        std::array<std::string, 6> _faceNames = { "", "", "", "", "", "" };
        Texture  _cubemap;
        Shader* _shader = nullptr;
        GLuint   _VAO = 0, _VBO = 0;
        bool     _loaded = false;

        void initGeometry();
    public:
        // Orden: right, left, top, bottom, front, back
        static constexpr std::array<const char*, 6> FACE_NAMES = {
            "Right", "Left", "Top", "Bottom", "Front", "Back"
        };

        Skybox() = default;
        ~Skybox();

        // Asignar una cara desde el inspector
        void setFace(int index, const std::string& path) {
            if (index >= 0 && index < 6)
                _faceNames[index] = path;
        }
        const std::string& getFace(int index) const { return _faceNames[index]; }

        bool load(Shader* shader);
        void render(Camera* cam);

        bool isLoaded() const { return _loaded; }

        void serialize(JsonSerializer& s) const override;
        void deserialize(JsonSerializer& s) override;
    };
}