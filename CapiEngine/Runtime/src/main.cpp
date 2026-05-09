#include <GLApplication.h>
#include <iostream>
#include <utils/logger.h>
#include <filesystem>
#include <managers/SceneManager.h>
#include <managers/InputManager.h>

#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <core/Scene.h>
#include <core/Camera.h>

#include <ResourcesLoader.h>

using namespace cme;

int main(int argc, char* argv[]) {
	std::filesystem::path exeDir = std::filesystem::path(argv[0]).parent_path();
	std::string name = std::filesystem::path(argv[0]).stem().string();

	if (!cme::GLApplication::Init(name)) {
		LOG_ERROR("No se pudo inicializar GLApplication");
		return -1;
	}

	runtime::ResourcesLoader rsc;
	rsc.init(exeDir, name);

	std::string path;
	if (argc > 1) {
		path = argv[1];
		LOG_INFO("Cargando escena desde argumento: " + path);
		cme::sceneM().loadScene(path);
	}
	else {
		std::string configFile = (exeDir / (name + ".json")).string();
		LOG_INFO("Buscando configuración en: " + configFile);
		
		if (!std::filesystem::exists(configFile)) {
			LOG_ERROR("Archivo de configuración no encontrado: " + configFile);
			LOG_ERROR("Crear un archivo " + name + ".json en la carpeta del ejecutable con el siguiente contenido:");
			LOG_ERROR(R"({"startScene": "assets/scenes/nombredelescena.json"})");
			return -1;
		}

		JsonSerializer s;
		s.load(configFile);

		std::string startScenePath = s.readString("startScene");
		if (startScenePath.empty()) {
			LOG_ERROR("La clave 'startScene' no existe en " + name + ".json");
			return -1;
		}

		std::string fullPath = (exeDir / startScenePath).string();
		if (!std::filesystem::exists(fullPath)) {
			LOG_ERROR("Escena no encontrada: " + fullPath);
			return -1;
		}

		LOG_INFO("Cargando escena: " + fullPath);
		cme::sceneM().loadScene(fullPath);
	}

	cme::gla().run();
	cme::gla().Release();

    return 0;
}
