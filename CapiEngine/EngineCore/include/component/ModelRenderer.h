#pragma once
#include <ec/component.h>
#include <string>
#include <array>
#include <glm/fwd.hpp>
#include <surface/Material.h>


namespace cme {
    class Model;
    class Camera;
    class Transform;
    class JsonSerializer;

    /// @brief Componente que renderiza un modelo 3D cargado con Assimp.
    ///        Equivalente a MeshRenderer pero para modelos multi-submesh.
    class ModelRenderer : public ec::Component, public ec::RenderComponent {
    private:
        Model*      _model = nullptr;   // owned
        Camera*     _cam   = nullptr;   // borrowed (de la escena)
        Transform*  _tr    = nullptr;   // borrowed (del mismo entity)

        std::string _modelPath;         // para serialización y recarga

    public:
        __CMPID_DECL__(ec::comp::MODEL_RENDERER)  // necesita registrarse en el enum comp

        ModelRenderer() = default;
        ~ModelRenderer();

        void initComponent() override;
        void render() const override;

        void serialize(JsonSerializer& s) const override;
        void deserialize(JsonSerializer& s) override;
        std::string serializeID() const override { return "ModelRenderer"; }

        void getLocalAABB(glm::vec3& outMin, glm::vec3& outMax) const;

        bool hasModel() const { return _model != nullptr; }
        Model* model() { return _model; }
        void setModel(Model* m) { _model = m; }
    };

} // namespace cme
