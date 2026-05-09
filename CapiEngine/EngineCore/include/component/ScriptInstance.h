#pragma once
#include <sol/sol.hpp>
#include <string>
#include <utils/logger.h>

namespace cme {
    struct ScriptInstance {
        std::string filepath;
        std::string name;

        sol::table  instance;
        sol::function startFunc;
        sol::function updateFunc;
        //bool _loaded = false;

        bool load(sol::state& lua);
        bool reload(sol::state& lua) {
            clear();
            return load(lua);
        }
        void start();
        void update();

        void clear() {
            try {
                instance = sol::lua_nil;
                startFunc = sol::lua_nil;
                updateFunc = sol::lua_nil;
            } catch (const std::exception& e) {
                // Silenciar excepciones durante limpieza si el state fue destruido
                LOG_WARN("Exception while clearing script instance: " + std::string(e.what()));
            }
        }
    };
}