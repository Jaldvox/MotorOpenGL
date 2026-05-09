#include <component/LuaScript.h>
#include <ec/entity.h>
#include <core/Scene.h>
#include <managers/ResourceManager.h>
#include <managers/SceneManager.h>

namespace cme {
    void LuaScript::initComponent() {
        if (auto entity = _entity.lock()) {
            _lua = &entity->getScene()->getLuaState();
        }
    }

    void LuaScript::addScript(ScriptInstance& script) {
        // Asegurar que tenemos una referencia valida al lua state
        if (!_lua) {
            if (auto entity = _entity.lock()) {
                _lua = &entity->getScene()->getLuaState();
            } else {
                auto* l = &sceneM().activeScene()->getLuaState();
                if (!l) {
                    LOG_ERROR("No se pudo obtener lua state para addScript");
                    return;
                }
                _lua = l;
            }
        }

        _scripts.push_back(script);
    }

    void LuaScript::removeScript(const std::string& path) {
        std::erase_if(_scripts, [&](const ScriptInstance& s) {
            return s.filepath == path;
            });
    }

    void LuaScript::start() {
        try {
            for (auto& s : _scripts) s.start();
        } catch (const std::exception& e) {
            LOG_ERROR("Error en script start: " + std::string(e.what()));
        }
    }

    void LuaScript::update() {
        try {
            for (auto& s : _scripts) s.update();
        } catch (const std::exception& e) {
            LOG_ERROR("Error en script update: " + std::string(e.what()));
        }
    }

    void LuaScript::serialize(cme::JsonSerializer& s) const {
        s.beginArray("scripts");
        for (auto& script : _scripts) {
            s.pushObjectToArray();
            s.write("name", script.name);
            s.endScope();
        }
        s.endScope();
    }

    void LuaScript::deserialize(cme::JsonSerializer& s) {
        s.beginArray("scripts");
        size_t n = s.getArraySize();
        for (size_t i = 0; i < n; i++) {
            s.enterElement(i);
            auto name = s.readString("name");
            ScriptInstance* script = rscrM().getScript(name);
            if (script) addScript(*script);
            else LOG_ERROR("Script no encontrado al deserializar: " + name);
            s.endScope();
        }
        s.endScope();
    }
}