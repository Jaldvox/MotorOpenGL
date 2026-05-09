#pragma once
#include <filesystem>
#include <unordered_set>
#include <variant>
#include <array>
#include <string>

namespace fs = std::filesystem;

namespace cme::runtime {
	constexpr std::array<std::string_view, 5> IMAGE_EXTENSIONS = {
		".png", ".jpg", ".jpeg", ".bmp", ".tga"
	};

	constexpr std::array<std::string_view, 5> SHADER_EXTENSIONS = {
		".frag", ".vert"
	};

	enum class ResourceType { Shader, Texture, Script, Unknown };

	class ResourcesLoader {
	public:
		ResourcesLoader() = default;
		bool init(fs::path projectPath, std::string& projectName);

	private:
		void loadFilesRecursive(const fs::path& root);
		void addResourceToCore(const ResourceType& type, const fs::path& file);
		ResourceType getFileType(const fs::path& file);
	};
}