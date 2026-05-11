#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLApplication.h"
#include <utils/logger.h>

#include <managers/ResourceManager.h>
#include <managers/SceneManager.h>
#include <managers/InputManager.h>
#include <core/Scene.h>
#include <core/Camera.h>

#include <register/ComponentRegistry.h>
#include <component/Transform.h>
#include <component/MeshRenderer.h>
#include <component/Light.h>
#include <component/LuaScript.h>
#include <component/ModelRenderer.h>

#include <sol/sol.hpp>

namespace cme {
	GLApplication::~GLApplication() {
		if (SceneManager::HasInstance()) {
			SceneManager::Release();
		}

		if (InputManager::HasInstance()) {
			InputManager::Release();
		}

		if (ResourceManager::HasInstance()) {
			ResourceManager::Release();
		}

		if (Logger::HasInstance()) {
			Logger::Release();
		}

		glfwDestroyWindow(_window);
		glfwTerminate();
	}

	bool GLApplication::init(std::string name) {
		// inicializar GLFW
		if (glfwInit() == GLFW_FALSE) {
			return false;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
		_window = glfwCreateWindow(_width, _height, std::string(name).c_str(), NULL, NULL);
		if (_window == NULL)
		{
			LOG_ERROR("Failed to create GLFW window");
			return false;
		}

		GLFWimage images[1];
		int channels;
		// Asegúrate de poner la ruta a tu imagen original en PNG
		images[0].pixels = stbi_load("resources/capi.png", &images[0].width, &images[0].height, &channels, 4); // El 4 fuerza que se cargue con canal Alpha (RGBA)

		if (images[0].pixels) {
			glfwSetWindowIcon(_window, 1, images);
			stbi_image_free(images[0].pixels); // Liberamos la memoria de la imagen una vez subida a GLFW
		}
		else {
			LOG_WARN("GLApplication: No se pudo cargar el icono de la ventana desde assets/icono.png");
		}

		int w = 0, h = 0;
		glfwGetWindowSize(_window, &w, &h);
		_width = w;
		_height = h;

		glfwMakeContextCurrent(_window);
		// Inicializad GLAD que carga los punteros a las funciones
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			LOG_ERROR("Failed to initialize GLAD");
			return false;
		}

		// Init viewPort
		glViewport(0, 0, _width, _height);
		glfwSetFramebufferSizeCallback(_window, [](GLFWwindow* window, int width, int height) {
			glViewport(0, 0, width, height);
			gla()._width = width;
			gla()._height = height;

			sceneM().activeScene()->getCamera()->buildProjectionMat();
			});

		if (!loadManagers()) return false;

		return true;
	}

	bool GLApplication::loadManagers() {
		if (!Logger::Init()) {
			LOG_ERROR("Error al inicializar el Logger");
			return false;
		}

		if (!ResourceManager::Init()) {
			LOG_ERROR("Error al inicializar el Resource Manager");
			return false;
		}

		if (!SceneManager::Init()) {
			LOG_ERROR("Error al inicializar el Scene Manager");
			return false;
		}

		if (!InputManager::Init()) {
			LOG_ERROR("Error al inicializar el Scene Manager");
			return false;
		}

		glfwSetCursorPosCallback(_window, inpM().mouseCallback);

		// Registrar Componentes
		ComponentRegistry::registerComponent<Transform>("Transform");
		ComponentRegistry::registerComponent<MeshRenderer>("MeshRenderer");
		ComponentRegistry::registerComponent<Light>("Light");
		ComponentRegistry::registerComponent<LuaScript>("LuaScript");
		ComponentRegistry::registerComponent<ModelRenderer>("ModelRenderer");

		return true;
	}

	void GLApplication::run() {
		start();
		while (!glfwWindowShouldClose(_window))
		{
			update();
			sceneM().renderShadows();

			render();
			sceneM().activeScene()->refresh();
			swapAndTime();
		}
	}

	void GLApplication::update() {
		processInput();
		clearRender();
		sceneM().update();
	}

	void GLApplication::render() const {
		sceneM().render();
	}

	void GLApplication::start() {
		sceneM().defaultScene();
		sceneM().start();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_FRAMEBUFFER_SRGB);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}

	void GLApplication::swapAndTime() {
		glfwSwapBuffers(_window);
		glfwPollEvents();

		float currentFrame = glfwGetTime();
		_deltaTime = currentFrame - _lastFrame;
		_lastFrame = currentFrame;
	}

	void GLApplication::processInput() {
		inpM().proccessInput();
	}
	void GLApplication::clearRender() {
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
	}
}
