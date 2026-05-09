#pragma once
#include <ec/component.h>
#include <sol/sol.hpp>
#include <string>
#include <vector>
#include <component/ScriptInstance.h>

namespace cme {

    class LuaScript : public ec::Component, public ec::UpdateComponent
    {
    private:
        std::vector<ScriptInstance> _scripts;
        sol::state* _lua = nullptr;
    public:
        __CMPID_DECL__(ec::comp::LUA_SCRIPT)

        LuaScript() = default;
        ~LuaScript() {
            for (auto& s : _scripts)
                s.clear();
            _scripts.clear();
        }

        void initComponent() override;

        void start() override;
        void update() override;

        // API pública para el usuario/editor
        void        addScript(ScriptInstance& script);
        void        removeScript(const std::string& path);
        int         scriptCount() const { return _scripts.size(); }
        std::string getScriptPath(int i) const { return _scripts[i].filepath; }
        std::vector<ScriptInstance>& scripts() { return _scripts; }

        void reloadScripts();

        void serialize(JsonSerializer& s) const override;
        void deserialize(JsonSerializer& s)       override;

        std::string serializeID() const override { return "LuaScript"; }
    };
}
