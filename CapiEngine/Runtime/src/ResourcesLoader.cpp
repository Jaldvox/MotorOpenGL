#include <ResourcesLoader.h>
#include <managers/ResourceManager.h>
#include <managers/SceneManager.h>
#include <serialize/JsonSerializer.h>

namespace cme::runtime {
	bool ResourcesLoader::init(fs::path path, std::string& projectName) {
		// load project resources
		loadFilesRecursive(path / "assets");

		return true;
	}

	void ResourcesLoader::loadFilesRecursive(const fs::path& root) {
		// Si no existe la ruta, salimos para evitar crasheos
		if (!fs::exists(root)) return;

		// Si nos pasan un archivo suelto en vez de una carpeta, lo procesamos directo
		if (!fs::is_directory(root)) {
			addResourceToCore(getFileType(root), root);
			return;
		}

		// Iterador recursivo mágico de C++17
		for (const auto& entry : fs::recursive_directory_iterator(root)) {
			// Solo procesamos si es un archivo de verdad (ignoramos carpetas)
			if (entry.is_regular_file()) {
				ResourceType type = getFileType(entry.path());
				addResourceToCore(type, entry.path());
			}
		}
	}

	void ResourcesLoader::addResourceToCore(const ResourceType& type, const fs::path& file) {
		switch (type) {
		case ResourceType::Shader:
			rscrM().loadResource<Shader>(file);
			break;
		case ResourceType::Texture:
			rscrM().loadResource<Texture>(file);
			break;
		case ResourceType::Script:
			rscrM().loadResource<ScriptInstance>(file);
			break;

		default:
			break;
		}
	}


	ResourceType ResourcesLoader::getFileType(const fs::path& file) {
		if (!fs::is_directory(file)) {
			auto extension = file.extension().string();

			if (extension == ".lua") {
				return ResourceType::Script;
			}
			else if (std::count(SHADER_EXTENSIONS.begin(), SHADER_EXTENSIONS.end(), extension)) {
				return ResourceType::Shader;
			}
			else if (std::count(IMAGE_EXTENSIONS.begin(), IMAGE_EXTENSIONS.end(), extension)) {
				return ResourceType::Texture;
			}
		}

		return ResourceType::Unknown;
	}
}