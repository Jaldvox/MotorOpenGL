#include "core/Scene.h"
#include <ec/entity.h>
#include <core/Camera.h>
#include <managers/ResourceManager.h>
#include <ec/component.h>
#include <component/Transform.h>
#include <component/MeshRenderer.h>
#include <component/LuaScript.h>
#include <mesh/QuadMesh.h>
#include <mesh/CubeMesh.h>
#include <lighting/GlobalLight.h>
#include <managers/LightManager.h>
#include <register/LuaRegistry.h>
#include <lighting/Skybox.h>

namespace cme {
	Scene::Scene(std::string name) : _name(name) {
		_cam = new Camera();
		_globalLight = new GlobalLight();
		initLua();

		_skybox = std::make_shared<Skybox>();

		_shadowShader = rscrM().getShader("shadowMap");
		_shadowMap = std::make_unique<ShadowMap>(); // ← faltaba
		_shadowMap->init();

		_lightMatrix = calcLightSpaceMatrix();
	}

	Scene::~Scene() {
		// Limpiar entities ANTES de destruir el lua state
		// Esto es crítico porque los componentes LuaScript necesitan acceso al state
		for (auto& group : _gameObjectsByGroup) {
			for (auto& entity : group) {
				// Si la entity tiene LuaScript, limpiar scripts antes
				if (auto luaScript = entity->getComponent<LuaScript>()) {
					for (auto& s : luaScript->scripts())
						s.clear();
				}
			}
			group.clear();
		}


		_gizmos.clear();
		delete _cam;
		delete _globalLight;
	}

	void Scene::initLua() {
		_lua.open_libraries(
			sol::lib::base,
			sol::lib::math,
			sol::lib::string,
			sol::lib::table
		);

		registerAPI(_lua);
	}

	void Scene::refresh() {
		for (ec::groupID_t gId = 0; gId < ec::ent::maxGroupLayer; gId++) {
			auto& grpEnts = _gameObjectsByGroup[gId];
			grpEnts.erase(
				std::remove_if(grpEnts.begin(), grpEnts.end(),
					[](ec::entity_t e) {
						if (e->isAlive()) {
							return false;
						}
						else {
							e.reset();
							return true;
						}
					}),
				grpEnts.end());
		}
	}

	void Scene::render() const {
		for (int i = 0; i < ec::ent::maxGroupLayer; i++) {
			for (auto& gObj : _gameObjectsByGroup[i]) {
				if (gObj && gObj->active()) gObj->render();
			}
		}

		for (auto& go : _gizmos) {
			go->render();
		}

		_skybox->render(_cam);
	}

	void Scene::update() {
		for (int i = 0; i < ec::ent::maxGroupLayer; i++) {
			for (auto& gObj : _gameObjectsByGroup[i]) {
				if (gObj->active()) gObj->update();
			}
		}
	};

	void Scene::start() {
		for (int i = 0; i < ec::ent::maxGroupLayer; i++) {
			for (auto& gObj : _gameObjectsByGroup[i]) {
				if (gObj->active()) gObj->start();
			}
		}
	};

	void Scene::renderShadows() {
		_lightMatrix = calcLightSpaceMatrix();

		glViewport(0, 0, _shadowMap->width, _shadowMap->height);
		glBindFramebuffer(GL_FRAMEBUFFER, _shadowMap->fbo);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT); // evita "peter panning" (sombras desplazadas)

		_shadowShader->use();
		_shadowShader->setUniform("lightSpaceMatrix", _lightMatrix);

		for (int i = 0; i < ec::ent::maxGroupLayer; i++) {
			for (auto& gObj : _gameObjectsByGroup[i]) {
				if (!gObj->active()) continue; 
				auto mr = gObj->getComponent<MeshRenderer>();
				if (!mr) continue;

				mr->renderDepth(_shadowShader); // solo geometría, sin material
			}
		}

		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, gla().width(), gla().height());
	}

	glm::mat4 Scene::calcLightSpaceMatrix() {
		float size = 20.f;
		float near_plane = 1.0f;
		float far_plane = 100.0f;

		glm::mat4 lightProj = glm::ortho(-size, size, -size, size, near_plane, far_plane);

		glm::vec3 lightDir = glm::normalize(_globalLight->direction());
		glm::vec3 lightPos = -lightDir * 30.f;

		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		// Si la luz mira casi perfectamente hacia abajo o arriba en Y
		if (std::abs(lightDir.x) < 0.001f && std::abs(lightDir.z) < 0.001f) {
			up = glm::vec3(0.0f, 0.0f, 1.0f); // Cambiamos el vector Up para evitar el colapso
		}

		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.f), up);
		return lightProj * lightView;
	}


	std::shared_ptr<ec::Entity> Scene::addGameObject(Scene* scene, std::string name, ec::ent::groupID grID) {
		auto shPtr = std::make_shared<ec::Entity>(grID, scene, name);
		_gameObjectsByGroup[grID].push_back(shPtr);
		return shPtr;
	}

	ec::entity_t Scene::addGizmos() {
		auto shPtr = std::make_shared<ec::Entity>((ec::ent::groupID)0, this, "");
		_gizmos.push_back(shPtr);
		return shPtr;
	}

	void Scene::addCubeToScene() {
		auto c = addGameObject(this, "Cube", ec::ent::None);
		auto tr = c->addComponent<Transform>();
		tr->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		tr->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
		tr->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));

		c->addComponent<MeshRenderer>(new CubeMesh(rscrM().getShader("default")));
	}

	// --------- SERIALIZACION ------------

	void Scene::serialize(JsonSerializer& s) const {
		s.write("name", _name);
		s.beginArray("entities_groups");
		for (int i = 0; i < ec::ent::maxGroupLayer; i++) {
			s.pushObjectToArray();
			s.write("group", i);
			s.beginArray("entities");
			for (auto& gObj : _gameObjectsByGroup[i]) {
				gObj->serialize(s);
			}
			s.endScope(); // El del array de entidades
			s.endScope(); // El array de grupos
		}
		s.endScope(); // El array de grupos
		

		s.beginScope("globalLight");
		_globalLight->serialize(s);
		s.beginScope("skybox");
		_skybox->serialize(s);
		s.endScope();
		s.endScope();
	}

	void Scene::deserialize(JsonSerializer& s) {
		lightM().clearLights();

		_name = s.readString("name");
		s.beginArray("entities_groups");
		size_t numGroups = s.getArraySize();

		if (numGroups > ec::ent::maxGroupLayer) {
			LOG_ERROR("Hay mas grupos de entidades de los permitidos en el archivo de escena a cargar");
			return;
		}

		for (size_t i = 0; i < numGroups; i++) {
			s.enterElement(i);
			ec::groupID_t grpID = s.readInt("group");
			s.beginArray("entities");

			size_t numEntities = s.getArraySize();
			for (size_t j = 0; j < numEntities; j++) {
				s.enterElement(j);
				std::string entityName = s.readString("name");
				auto ent = addGameObject(this, entityName, (ec::ent::groupID)grpID);
				ent->deserialize(s);

				s.endScope(); // Salimos de la entidad j
			}
			s.endScope(); // Salimos del array entities
			s.endScope(); // Salimos del grupo 1
		}

		s.endScope();

		s.beginScope("globalLight");
		_globalLight->deserialize(s);
		s.beginScope("skybox");
		_skybox->deserialize(s);
		s.endScope();
		s.endScope();
	}

}