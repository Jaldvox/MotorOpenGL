#include <component/ScriptInstance.h>
#include <utils/logger.h>

namespace cme {
    bool ScriptInstance::load(sol::state& lua) {
        try {
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
        } catch (const std::exception& e) {
            LOG_ERROR("Excepción al cargar script " + filepath + ": " + std::string(e.what()));
            return false;
        }
    }

    void ScriptInstance::start() { 
        try {
            if (startFunc.valid()) startFunc(instance);
        } catch (const std::exception& e) {
            LOG_ERROR("Error ejecutando Start en script: " + std::string(e.what()));
        }
    }
    
    void ScriptInstance::update() { 
        try {
            if (updateFunc.valid()) updateFunc(instance);
        } catch (const std::exception& e) {
            LOG_ERROR("Error ejecutando Update en script: " + std::string(e.what()));
        }
    }
}