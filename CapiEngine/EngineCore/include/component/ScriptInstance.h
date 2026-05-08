#pragma once
#include <sol/sol.hpp>
#include <string>

namespace cme {
    struct ScriptInstance {
        std::string filepath;
        std::string name;

        sol::table  instance;
        sol::function startFunc;
        sol::function updateFunc;

        bool load(sol::state& lua);
        bool reload(sol::state& lua) {
            clear();
            return load(lua);
        }
        void start();
        void update();

        void clear() {
            instance = sol::lua_nil;
            startFunc = sol::lua_nil;
            updateFunc = sol::lua_nil;
        }
    };
}