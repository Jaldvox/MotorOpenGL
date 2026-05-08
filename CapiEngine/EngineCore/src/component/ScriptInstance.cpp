#include <component/ScriptInstance.h>
#include <utils/logger.h>

namespace cme {
    bool ScriptInstance::load(sol::state& lua) {
        sol::load_result file = lua.load_file(filepath);
        if (!file.valid()) {
            sol::error err = file;
            LOG_ERROR("Error cargando script: " + std::string(err.what()));
            return false;
        }

        sol::protected_function_result result = file();
        if (!result.valid()) {
            sol::error err = result;
            LOG_ERROR("Error ejecutando script: " + std::string(err.what()));
            return false;
        }

        if (result.get_type() != sol::type::table) {
            LOG_ERROR("El script no devolvió una tabla (falta return al final?): " + filepath);
            return false;
        }

        sol::table scriptClass = result.get<sol::table>();

        instance = lua.create_table();
        instance[sol::metatable_key] = scriptClass;

        startFunc = scriptClass["Start"];
        updateFunc = scriptClass["Update"];

        return true;
    }

    void ScriptInstance::start() { if (startFunc.valid())  startFunc(instance); }
    void ScriptInstance::update() { if (updateFunc.valid()) updateFunc(instance); }
}