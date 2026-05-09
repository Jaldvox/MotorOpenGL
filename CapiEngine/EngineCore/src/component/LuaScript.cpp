#include <component/LuaScript.h>
#include <ec/entity.h>
#include <core/Scene.h>
#include <managers/ResourceManager.h>
#include <managers/SceneManager.h>
#include <component/Light.h>

namespace cme {
    void LuaScript::initComponent() {
        if (auto entity = _entity.lock()) {
            _lua = &entity->getScene()->getLuaState();

            for (auto& s : _scripts) {
                s.reload(*_lua);
                // Inyectar referencias útiles en la instancia
                if (auto* tr = entity->getComponent<Transform>())
                    s.instance["transform"] = tr;
                if (auto* lt = entity->getComponent<Light>())
                    s.instance["light"] = lt;
                s.instance["entity"] = entity.get();
            }
        }
    }

    void LuaScript::addScript(ScriptInstance& script) {
        if (!_lua) {
            if (auto entity = _entity.lock()) {
                _lua = &entity->getScene()->getLuaState();
            }
            else {
                auto* l = &sceneM().activeScene()->getLuaState();
                if (!l) {
                    LOG_ERROR("No se pudo obtener lua state para addScript");
                    return;
                }
                _lua = l;
            }
        }

        // Hacemos una copia "limpia" (solo el nombre y la ruta)
        ScriptInstance newInstance;
        newInstance.filepath = script.filepath;
        newInstance.name = script.name;

        // LO CARGAMOS AQUÍ, en el estado de la escena actual
        if (newInstance.load(*_lua)) {
            // Inyectamos dependencias al vuelo si la entidad ya existe
            if (auto entity = _entity.lock()) {
                if (auto* tr = entity->getComponent<Transform>())
                    newInstance.instance["transform"] = tr;
                if (auto* lt = entity->getComponent<Light>())
                    newInstance.instance["light"] = lt;

                newInstance.instance["entity"] = entity.get();
            }
            _scripts.push_back(newInstance);
        }
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

            // Obtenemos solo el "molde" (las rutas) del manager
            ScriptInstance* scriptTemplate = rscrM().getScript(name);
            if (scriptTemplate) {
                addScript(*scriptTemplate); // Ahora addScript se encarga de clonarlo y compilarlo seguro
            }
            else {
                LOG_ERROR("Script no encontrado al deserializar: " + name);
            }
            s.endScope();
        }
        s.endScope();
    }
}