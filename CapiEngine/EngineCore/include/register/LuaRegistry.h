#pragma once
#include <GLFW/glfw3.h>

#include <GLApplication.h>
#include <component/Transform.h>
#include <component/MeshRenderer.h>
#include <component/Light.h>
#include <component/LuaScript.h>
#include <core/Camera.h>
#include <core/Scene.h>
#include <managers/InputManager.h>
#include <managers/SceneManager.h>
#include <core/KeyCode.h>
#include <ec/entity.h>

namespace cme {
	void registerInput(sol::state& lua);
	void registerGLM(sol::state& lua);
	void registerComponents(sol::state& lua);
	void registerExtras(sol::state& lua);

	inline void registerAPI(sol::state& lua) {
		registerInput(lua);
		registerGLM(lua);
		registerComponents(lua);
		registerExtras(lua);
	}

	void registerInput(sol::state& lua) {
		sol::table input = lua.create_named_table("Input");
		registerKeyCode(lua);

		input["isKeyPressed"] = [](KeyCode k) {
			return inpM().isKeyPressed(static_cast<int>(k));
			};
		input["isKeyReleased"] = [](KeyCode k) {
			return inpM().isKeyReleased(static_cast<int>(k));
			};
		input["isMousePressed"] = [](int button) {
			return inpM().isMouseButtonPressed(button);
			};
		input["mouseX"] = []() { return inpM().getMouseX(); };
		input["mouseY"] = []() { return inpM().getMouseY(); };
		input["viewportMouseX"] = []() { return inpM().getViewportMouseX(); };
		input["viewportMouseY"] = []() { return inpM().getViewportMouseY(); };

		// Botones del ratón como constantes
		input["MOUSE_LEFT"] = 0;
		input["MOUSE_RIGHT"] = 1;
		input["MOUSE_MIDDLE"] = 2;

		input["lockCursor"] = [](bool lock) {
			if (lock) {
				glfwSetInputMode(gla().window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else {
				glfwSetInputMode(gla().window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
		};
	}

	void registerGLM(sol::state& lua) {
		lua.new_usertype<glm::vec3>("Vec3",
			sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
			"x", &glm::vec3::x,
			"y", &glm::vec3::y,
			"z", &glm::vec3::z,
			sol::meta_function::addition, [](const glm::vec3& a, const glm::vec3& b) { return a + b; },
			sol::meta_function::subtraction, [](const glm::vec3& a, const glm::vec3& b) { return a - b; },
			sol::meta_function::multiplication, [](const glm::vec3& a, float s) { return a * s; },
			sol::meta_function::to_string, [](const glm::vec3& v) {
				return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
			}
		);
	}

	void registerComponents(sol::state& lua) {
		lua.new_usertype<Transform>("Transform",
			"position", sol::property(
				static_cast<const glm::vec3 & (Transform::*)() const>(&Transform::getPosition),
				static_cast<void (Transform::*)(const glm::vec3&)>(&Transform::setPosition)
			),
			"scale", sol::property(
				static_cast<const glm::vec3 & (Transform::*)() const>(&Transform::getScale),
				static_cast<void (Transform::*)(const glm::vec3&)>(&Transform::setScale)
			),
			"rotation", sol::property(
				static_cast<const glm::vec3 & (Transform::*)() const>(&Transform::getRotation),
				static_cast<void (Transform::*)(const glm::vec3&)>(&Transform::setRotation)
			)
		);

		lua.new_usertype<PointLight>("PointLight",
			"color", &PointLight::color,
			"intensity", &PointLight::intensity,
			"constant", &PointLight::constant,
			"linear", &PointLight::linear,
			"quadratic", &PointLight::quadratic
		);

		lua.new_usertype<Light>("Light",
			"pointLight", &Light::_pointLight,
			"position", sol::readonly_property(&Light::getPosition)
		);
	}

	void registerExtras(sol::state& lua) {
		lua.new_usertype<Camera>("Camera",
			"position", sol::property(&Camera::getPosition, &Camera::setPosition),
			"lookAt", &Camera::setCameraLookAt,
			"direction", sol::readonly_property(&Camera::getCameraFront),
			"up", sol::readonly_property(&Camera::getCameraUp),
			"right", sol::readonly_property([](Camera& cam) {
				return glm::normalize(glm::cross(cam.getCameraFront(), cam.getCameraUp()));
			})
		);

		sol::table scene = lua.create_named_table("Scene");
		scene["load"] = [](const std::string& path) {
			std::string p = path;
			sceneM().loadScene(p);
			};
		scene["camera"] = []() {
			return sceneM().activeScene()->getCamera();
			};

		sol::table time = lua.create_named_table("Time");
		time["deltaTime"] = []() { return gla().deltaTime(); };
	}
}