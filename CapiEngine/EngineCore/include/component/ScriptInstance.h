#pragma once
#include <sol/sol.hpp>
#include <string>
#include <utils/logger.h>

namespace cme {
    struct ScriptInstance {
        std::string filepath;
        std::string name;

        sol::table  instance;
        sol::protected_function startFunc;
        sol::protected_function updateFunc;
        //bool _loaded = false;

        bool load(sol::state& lua);
        bool reload(sol::state& lua) {
            clear();
            return load(lua);
        }
        void start();
        void update();

        void clear() {
            instance = sol::table{};    // reset sin tocar Lua
            startFunc = sol::function{};
            updateFunc = sol::function{};
        }
    };
}