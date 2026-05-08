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
        if (!_lua) {
            auto* l = &sceneM().activeScene()->getLuaState();
            if (l) {
                _scripts.push_back(script);
            }
            return;
        }

        _scripts.push_back(script);
    }

    void LuaScript::removeScript(const std::string& path) {
        std::erase_if(_scripts, [&](const ScriptInstance& s) {
            return s.filepath == path;
            });
    }

    void LuaScript::start() {
        for (auto& s : _scripts) s.start();
    }

    void LuaScript::update() {
        for (auto& s : _scripts) s.update();
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
            addScript(rscrM().getScript(name));
            s.endScope();
        }
        s.endScope();
    }
}