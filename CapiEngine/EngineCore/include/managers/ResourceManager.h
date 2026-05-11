#pragma once
#include <unordered_map>
#include <utils/Singleton.h>
#include <surface/Shader.h>
#include <surface/Texture.h>
#include <filesystem>
#include <sol/sol.hpp>
#include <component/ScriptInstance.h>
#include <mesh/Model.h>

namespace fs = std::filesystem;

namespace cme {
	/// @brief Se encarga de la gesiton de recursos del proyecto, desde shaders hasta imagenes. Utiliza el patron Singleton y se puede acceder a su instancia con rscrM()
	class ResourceManager : public Singleton<ResourceManager>
	{
		friend class Singleton<ResourceManager>;
	private:
		// ----- SHADERS -----
		std::unordered_map<std::string, std::unique_ptr<Shader>> _shadersMap;
		std::vector<Shader*> _shaders;
		std::vector<std::string> _shaderNames;

		// ----- TEXTURAS -----
		std::unordered_map<std::string, std::unique_ptr<Texture>> _texturesMap;
		std::vector<std::string> _texturesNames;

		// ----- SCRIPTS -----
		std::unordered_map<std::string, ScriptInstance> _scripts;
		std::vector<std::string> _scriptsNames;

		std::unordered_map<std::string, std::unique_ptr<Model>> _modelsMap;
		std::vector<std::string> _modelNames;
		// Texturas registradas externamente (el owner real es SubMesh::ownedTextures).
		// Mapa separado para no mezclar ownership con _texturesMap.
		std::unordered_map<std::string, Texture*> _borrowedTextures;

	public:
		virtual ~ResourceManager();

		// cannot copy/move
		ResourceManager(ResourceManager&) = delete;
		ResourceManager(ResourceManager&&) = delete;
		ResourceManager& operator=(ResourceManager&) = delete;
		ResourceManager& operator=(ResourceManager&&) = delete;

		/// @brief Busca en los recursos el shader pedido mediante la key, que es el nombre del archivo sin la extensión
		/// @param key La clave del shader
		/// @return Devuelve un puntero inteligente al shader
		Shader* getShader(std::string key);
		Texture* getTexture(std::string key);
		ScriptInstance* getScript(std::string& key);
		Model* getModel(const std::string& key);

		void registerTexture(const std::string& name, Texture* tex);

		/// @brief Busca todos los shaders cargados y los almacena en un vector
		/// @return Un vector de shaders
		std::vector<Shader*> getAllShaders();
		/// @brief Busca el nombre de todos los shader cargados
		/// @return Un vector de nombres
		std::vector<std::string> getAllShaderNames();
		std::vector<std::string> getAllTextureNames();
		std::vector<std::string> getAllScriptNames();
		std::vector<std::string> getAllModelNames() const { return _modelNames; }

	private:
		ResourceManager() = default;

		void loadShader(fs::path file);
		void loadTexture(fs::path file);
		void loadScript(const fs::path& path);
		Model* loadModel(const fs::path& file, const std::string& shaderName = "default");

		/// @brief Inicializa el Resource Manager
		/// @return False si falla
		bool init();
	public:
		template<typename T, typename ...Targs>
		void loadResource(const fs::path& file, Targs &&...args) {
			std::string name = file.stem().string(); // nombre sin extensión

			if constexpr (std::is_same_v<T, Shader>) {
				if (std::find(_shaderNames.begin(), _shaderNames.end(), name) != _shaderNames.end()) return; // ya cargado
				loadShader(file);
			}
			else if constexpr (std::is_same_v<T, Texture>) {
				if (std::find(_texturesNames.begin(), _texturesNames.end(), name) != _texturesNames.end()) return;
				loadTexture(file);
			}
			else if constexpr (std::is_same_v<T, ScriptInstance>) {
				loadScript(file);
			}
			else if constexpr (std::is_same_v<T, Model>) {
				loadModel(file);
			}
			else {
				static_assert(!sizeof(T), "Tipo de recurso no soportado");
			}
		}
	};

	/// @brief Devuelve la instancia de ResourceManager mediante el uso del patrón Singleton
	/// @return La instancia de ResourceManager
	inline ResourceManager& rscrM() {
		return *ResourceManager::Instance();
	}
}