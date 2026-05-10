#include "EditorApp.h"
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#endif

#include <core/Scene.h>
#include <core/Camera.h>
#include <utils/Raycast.h>
#include <utils/FileExplorer.h>

#include <managers/UIManager.h>
#include <managers/ResourceManager.h>
#include <component/Transform.h>
#include <component/MeshRenderer.h>
#include <component/LuaScript.h>
#include <mesh/QuadMesh.h>
#include <windows/InspectorWindow.h>
#include <project/ProjectLoader.h>
#include <project/ProjectFileData.h>
#include <project/ProjectBuilder.h>

namespace cme::editor {

	EditorApp::~EditorApp() {
		if (GLApplication::HasInstance()) {
			GLApplication::Release();
		}

		if (ProjectLoader::HasInstance()) {
			ProjectLoader::Release();
		}

		delete _projectData;
	}

	bool EditorApp::init(fs::path enginePath, fs::path projectPath, fs::path projFile) {
		_projectData = new ProjectFileData(projFile);

		if (!GLApplication::Init("Capi Engine - " + _projectData->projectData().name)) {
			LOG_ERROR("Error al inicializar el GLApplication");
			return false;
		}

		_ui = std::make_shared<UIManager>();
		if (!_ui->initCoreUI(gla().window(), getEngineDataPath())) {
			LOG_ERROR("Error al inicializar la interfaz del motor");
			return false;
		}

		_builder = std::make_shared<ProjectBuilder>();

		// Configuramos eventos de la UI
		_ui->setCreateCubeCallback([]() {
			sceneM().activeScene()->addCubeToScene();
			});

		_enginePath = enginePath;
		_projectPath = projectPath;
		_projectFile = projFile;

		return true;
	}

	void EditorApp::run() {
		auto ep = enginePath();
		auto pp = projectPath();
		if (!ProjectLoader::Init(ep, pp)) {
			LOG_ERROR("Error al inicializar el Proyecto (ProjectLoader");
			return;
		}

		sceneM().setDoStart(false);
		gla().start();
		_ui->start();

		sceneM().loadScene(_projectData->projectData().startScene);

		createShortcuts();
		stateChangers();
		createGizmos();

		while (!glfwWindowShouldClose(gla().window())) {
			gla().clearRender();
			gla().processInput();

			if (inpM().getCurrentState() == CME_STATE_VIEWPORT_MOVING) {
				Camera* cam = sceneM().activeScene()->getCamera();
				cam->setCameraLookAt(inpM().getMouseX(), inpM().getMouseY());
			}

			sceneM().renderShadows();

			_ui->bind();
			sceneM().render();
			_ui->unbind();

			glEnable(GL_FRAMEBUFFER_SRGB);
			_ui->render();
			sceneM().activeScene()->refresh();

			gla().swapAndTime();
		}
	}

	void EditorApp::createGizmos() {
		Shader* gridShader = rscrM().getShader("grid");
		// 2. Crear la entidad
		ec::entity_t gridEntity = sceneM().activeScene()->addGizmos();

		auto tr = gridEntity->addComponent<Transform>();
		tr->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		tr->setScale(glm::vec3(500.0f, 500.0f, 1.0f)); // Un plano muy grande
		tr->setRotation(glm::vec3(-90.0f, 0.0f, 0.0f)); // Rotarlo para que acueste en Y=0 (depende de tu QuadMesh)

		gridEntity->addComponent<MeshRenderer>(new QuadMesh(gridShader));
	}

	void EditorApp::createShortcuts() {
		// ----- ATAJOS VENTANA MOTOR -----
		std::vector<int> saveFileKeys = { GLFW_KEY_LEFT_CONTROL, GLFW_KEY_S };
		Shortcut saveFile(saveFileKeys, [this]() {
			_ui->saveSceneFileContext();
		}, CME_STATE_NORMAL);
		inpM().addShortcut(saveFile);

		std::vector<int> loadFileKeys = { GLFW_KEY_LEFT_CONTROL, GLFW_KEY_L };
		Shortcut loadFile(loadFileKeys, [this]() {
			_ui->loadSceneFileContext();
		}, CME_STATE_NORMAL);
		inpM().addShortcut(loadFile);

		// ----- MOVIMIENTO DEL VIEWPORT -----
		// Usamos { GLFW_KEY_X } directamente en el constructor
		Shortcut cameraMoveW({ GLFW_KEY_W }, []() {
			Camera* cam = sceneM().activeScene()->getCamera();
			cam->setPosition(cam->getPosition() + cam->movementSpeed() * cam->getCameraFront() * gla().deltaTime());
			}, CME_STATE_VIEWPORT_MOVING, true);

		Shortcut cameraMoveS({ GLFW_KEY_S }, []() {
			Camera* cam = sceneM().activeScene()->getCamera();
			cam->setPosition(cam->getPosition() - cam->movementSpeed() * cam->getCameraFront() * gla().deltaTime());
			}, CME_STATE_VIEWPORT_MOVING, true);

		Shortcut cameraMoveA({ GLFW_KEY_A }, []() {
			Camera* cam = sceneM().activeScene()->getCamera();
			glm::vec3 right = glm::normalize(glm::cross(cam->getCameraFront(), cam->getCameraUp()));
			cam->setPosition(cam->getPosition() - right * cam->movementSpeed() * gla().deltaTime());
			}, CME_STATE_VIEWPORT_MOVING, true);

		Shortcut cameraMoveD({ GLFW_KEY_D }, []() {
			Camera* cam = sceneM().activeScene()->getCamera();
			glm::vec3 right = glm::normalize(glm::cross(cam->getCameraFront(), cam->getCameraUp()));
			cam->setPosition(cam->getPosition() + right * cam->movementSpeed() * gla().deltaTime());
			}, CME_STATE_VIEWPORT_MOVING, true);

		Shortcut fastMove({ GLFW_KEY_LEFT_SHIFT }, []() {
			Camera* cam = sceneM().activeScene()->getCamera();
			float movspeed = cam->movementSpeed();
			movspeed = cam->FAST_SPEED;
			cam->setMovementSpeed(movspeed);
			}, CME_STATE_VIEWPORT_MOVING, true);

		Shortcut normalMove({ GLFW_KEY_LEFT_SHIFT }, []() {
			Camera* cam = sceneM().activeScene()->getCamera();
			float movspeed = cam->movementSpeed();
			movspeed = cam->SLOW_SPEED;
			cam->setMovementSpeed(movspeed);
			}, CME_STATE_VIEWPORT_MOVING, true, GLFW_RELEASE);

		std::vector<int> reloadKeys = { GLFW_KEY_LEFT_CONTROL, GLFW_KEY_R };
		Shortcut reloadScript(reloadKeys, [this]() {
			reloadScripts();
		}, CME_STATE_NORMAL);

		inpM().addShortcut(reloadScript);

		inpM().addShortcut(cameraMoveW);
		inpM().addShortcut(cameraMoveS);
		inpM().addShortcut(cameraMoveA);
		inpM().addShortcut(cameraMoveD);
		inpM().addShortcut(fastMove);
		inpM().addShortcut(normalMove);
	}

	void EditorApp::stateChangers() {
		StateChanger toMoving([]() {
			return inpM().isViewportHovered() && glfwGetMouseButton(gla().window(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
			},
			CME_STATE_VIEWPORT_MOVING,
			[]() {
				sceneM().activeScene()->getCamera()->firstMove();
				glfwSetInputMode(gla().window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			});

		StateChanger toNormal([]() {
			return glfwGetMouseButton(gla().window(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE;
			},
			CME_STATE_NORMAL,
			[]() {
				glfwSetInputMode(gla().window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			});

		MouseEvent selectObjInViewPortr([]() {
			return inpM().isViewportHovered() && glfwGetMouseButton(gla().window(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
			}, [this]() {
				Raycast ray;
				auto ent = ray.castRay();
				auto win = _ui->getWindow(INSPECTOR);
				if (auto lockedWin = win.lock()) {
					InspectorWindow& inspector = static_cast<InspectorWindow&>(*lockedWin);
					inspector.changeDisplayEntity(ent);
					LOG_INFO("Cambiando entidad del inspector");
				}
				}, GLFW_MOUSE_BUTTON_LEFT);

			inpM().addMouseEvent(selectObjInViewPortr);

			inpM().addStateChanger(toMoving);
			inpM().addStateChanger(toNormal);
	}

	void EditorApp::reloadScripts() {
		for (int i = 0; i < ec::ent::maxGroupLayer; i++) {
			for (auto& gObj : sceneM().activeScene()->getSceneObjects()[i]) {
				if (auto luaScript = gObj->getComponent<LuaScript>()) {
					luaScript->reloadScripts();
				}
			}
		}
		LOG_INFO("¡Todos los scripts de la escena han sido recargados!");
	}

	fs::path EditorApp::getEngineDataPath() {
		// Dev: variable de entorno override
		if (const char* dev = std::getenv("ENGINE_DATA_PATH"))
			return fs::path(dev);

#ifdef _WIN32
		// C:/Users/Usuario/AppData/Roaming/TuEngine
		const char* appdata = std::getenv("APPDATA");
		if (appdata) return fs::path(appdata) / "CapiEngine";

#elif __APPLE__
		// /Users/Usuario/Library/Application Support/TuEngine
		const char* home = std::getenv("HOME");
		if (home) return fs::path(home) / "Library" / "Application Support" / "CapiEngine";

#elif __linux__
		// Sigue el estándar XDG: ~/.local/share/TuEngine
		// Pero respeta si el usuario tiene XDG_DATA_HOME personalizado
		const char* xdg = std::getenv("XDG_DATA_HOME");
		if (xdg) return fs::path(xdg) / "CapiEngine";

		const char* home = std::getenv("HOME");
		if (home) return fs::path(home) / ".local" / "share" / "CapiEngine";
#endif

		// Fallback universal: junto al exe
		return _enginePath;
	}
}