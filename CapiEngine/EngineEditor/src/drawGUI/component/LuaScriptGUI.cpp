#include <drawGUI/component/LuaScriptGUI.h>
#include <component/LuaScript.h>
#include <utils/ImGuiUtils.h>
#include <imgui.h>
#include <managers/ResourceManager.h>

namespace cme::editor {
	LuaScriptGUI::LuaScriptGUI(LuaScript* script) : _lua(script) {}

	void LuaScriptGUI::drawOnInspector() {

		std::vector<ScriptInstance> scriptsToErase;
		if (ImGui::CollapsingHeader("Lua Script")) {
			for (const auto& script : _lua->scripts()) {
				ImGui::Text(script.name.c_str());
				ImGui::SameLine();

				if (ImGui::Button("Erase")) {
					scriptsToErase.push_back(script);
				}
			}

			for (auto& s : scriptsToErase) {
				_lua->removeScript(s.filepath);
			}

			if (ImGui::Button("+ Add Script"))
				ImGui::OpenPopup("ScriptPickerPopup");

			// El popup
			if (ImGui::BeginPopup("ScriptPickerPopup")) {
				ImGui::Text("Selecciona un script:");
				ImGui::Separator();

				for (auto& name : rscrM().getAllScriptNames()) {
					if (ImGui::Selectable(name.c_str())) {
						_lua->addScript(*rscrM().getScript(name));
						ImGui::CloseCurrentPopup();
					}
				}

				ImGui::EndPopup();
			}
		}
	}
}
