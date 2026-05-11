#include <project/ProjectBuilder.h>
#include <filesystem>
#include <fstream>
#include <EditorApp.h>
#include <project/ProjectFileData.h>
#include <serialize/JsonSerializer.h>

namespace fs = std::filesystem;

namespace cme::editor {
    void ProjectBuilder::build() {

        std::string buildDir = editor().projectPath().string() + "/build/";

        // Crear carpeta build
        fs::create_directories(buildDir);
        fs::path destCorePath = fs::path(buildDir) / "core" / "shaders";

        // 2. Crear la carpeta (y todas las subcarpetas necesarias)
        fs::create_directories(destCorePath);

        std::string jsonPath = buildDir + editor().projectData()->projectData().name + ".json";

        JsonSerializer s;
        s.write("startScene", editor().projectData()->projectData().startScene);
        s.write("projectName", editor().projectData()->projectData().name);
        s.save(jsonPath);

        // Copiar Runtime.exe renombrado
        fs::copy_file(
            editor().enginePath() / "EngineRuntime.exe",
            buildDir + editor().projectData()->projectData().name + ".exe",
            fs::copy_options::overwrite_existing
        );

        // Copiar assets
        fs::copy(
            editor().projectPath() / "assets",
            buildDir + "assets",
            fs::copy_options::recursive |
            fs::copy_options::overwrite_existing
        );

        // Copiar shaders del core
        auto p = editor().enginePath() / "resources" / "shaders";
        if (!fs::exists(p)) {
            LOG_ERROR("No se puede hacer build. NO existe resources");
            return;
        }
        fs::copy(
            p,
            destCorePath,
            fs::copy_options::recursive |
            fs::copy_options::overwrite_existing
        );

        // Copiar DLLs necesarias (glfw, etc)
        fs::copy_file(
            editor().enginePath() / "glfw3.dll",
            buildDir + "glfw3.dll",
            fs::copy_options::overwrite_existing
        );

        fs::copy_file(
            editor().enginePath() / "assimp-vc145-mt.dll",
            buildDir + "assimp-vc145-mt.dll",
            fs::copy_options::overwrite_existing
        );
    }
}