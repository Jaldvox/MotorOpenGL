#include <component/ModelRenderer.h>
#include <mesh/Model.h>
#include <core/Camera.h>
#include <core/Scene.h>
#include <component/Transform.h>
#include <serialize/JsonSerializer.h>
#include <utils/logger.h>

#include <ec/entity.h>
#include <managers/ResourceManager.h>

namespace cme {

    ModelRenderer::~ModelRenderer() {
        delete _model;
    }

    void ModelRenderer::initComponent() {
        if (auto ent = _entity.lock()) {
            _cam = ent->getScene()->getCamera();
            _tr  = ent->getComponent<Transform>();
            assert(_tr != nullptr);
        }
    }

    void ModelRenderer::render() const {
        if (!_model || !_cam || !_tr) return;

        if (auto entSp = _entity.lock()) {
            _model->render(_tr->getModelMatrix(), _cam, entSp);
        }
    }

    void ModelRenderer::getLocalAABB(glm::vec3& outMin, glm::vec3& outMax) const {
        if (_model)
            _model->getLocalAABB(outMin, outMax);
        else {
            outMin = outMax = glm::vec3(0.0f);
        }
    }

    void ModelRenderer::serialize(JsonSerializer& s) const {
        s.write("modelName", _model->name());
    }

    void ModelRenderer::deserialize(JsonSerializer& s) {
        auto name = s.readString("modelName");
        _model = rscrM().getModel(name);
    }
}
