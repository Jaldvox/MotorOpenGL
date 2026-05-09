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

void inputs();

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

	inputs();

	cme::gla().run();
	cme::gla().Release();

    return 0;
}

void inputs() {
	// ----- MOVIMIENTO DEL VIEWPORT -----
	std::vector<int> key = { GLFW_KEY_W };
	cme::Shortcut cameraMoveW(key, []() {
		Camera* cam = sceneM().activeScene()->getCamera();
		glm::vec3 cameraPos = cam->getPosition();
		glm::vec3 cameraFront = cam->getCameraFront();

		cameraPos += cam->movementSpeed() * cameraFront * gla().deltaTime();
		cam->setPosition(cameraPos);
		}, CME_STATE_NORMAL);

	key = { GLFW_KEY_S };
	Shortcut cameraMoveS(key, []() {
		Camera* cam = sceneM().activeScene()->getCamera();
		glm::vec3 cameraPos = cam->getPosition();
		glm::vec3 cameraFront = cam->getCameraFront();

		cameraPos -= cam->movementSpeed() * cameraFront * gla().deltaTime();
		cam->setPosition(cameraPos);
		}, CME_STATE_NORMAL);

	key = { GLFW_KEY_A };
	Shortcut cameraMoveA(key, []() {
		Camera* cam = sceneM().activeScene()->getCamera();
		glm::vec3 cameraPos = cam->getPosition();
		glm::vec3 cameraFront = cam->getCameraFront();
		glm::vec3 cameraUp = cam->getCameraUp();

		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cam->movementSpeed() * gla().deltaTime();
		cam->setPosition(cameraPos);
		}, CME_STATE_NORMAL);

	key = { GLFW_KEY_D };
	Shortcut cameraMoveD(key, []() {
		Camera* cam = sceneM().activeScene()->getCamera();
		glm::vec3 cameraPos = cam->getPosition();
		glm::vec3 cameraFront = cam->getCameraFront();
		glm::vec3 cameraUp = cam->getCameraUp();

		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cam->movementSpeed() * gla().deltaTime();
		cam->setPosition(cameraPos);
		}, CME_STATE_NORMAL);

	key = { GLFW_KEY_LEFT_SHIFT };
	Shortcut fastMove(key, []() {
		Camera* cam = sceneM().activeScene()->getCamera();
		float movspeed = cam->movementSpeed();
		movspeed = cam->FAST_SPEED;
		cam->setMovementSpeed(movspeed);
		}, CME_STATE_NORMAL);

	key = { GLFW_KEY_LEFT_SHIFT };
	Shortcut normalMove(key, []() {
		Camera* cam = sceneM().activeScene()->getCamera();
		float movspeed = cam->movementSpeed();
		movspeed = cam->SLOW_SPEED;
		cam->setMovementSpeed(movspeed);
		}, CME_STATE_NORMAL, GLFW_RELEASE);

	inpM().addShortcut(cameraMoveW);
	inpM().addShortcut(cameraMoveS);
	inpM().addShortcut(cameraMoveA);
	inpM().addShortcut(cameraMoveD);
	inpM().addShortcut(fastMove);
	inpM().addShortcut(normalMove);
}