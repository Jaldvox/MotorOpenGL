#include <drawGUI/component/GlobalLightGUI.h>
#include <lighting/GlobalLight.h>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <lighting/Skybox.h>
#include <managers/SceneManager.h>
#include <managers/ResourceManager.h>
#include <core/Scene.h>

namespace cme::editor {
	GlobalLightGUI::GlobalLightGUI(GlobalLight* globalLight) : _globalLight(globalLight) {
	}

	void GlobalLightGUI::drawOnInspector() {
		ImGui::Text("Light Color");
		ImGui::SameLine(0.0f, 10.0f);
		ImGui::ColorEdit3("##Light Color", glm::value_ptr(_globalLight->_color));

        bool changed = false;
        changed |= ImGui::SliderFloat("Azimuth", &_globalLight->_azimuth, -180.0f, 180.0f, "%.1f deg");
        changed |= ImGui::SliderFloat("Elevation", &_globalLight->_elevation, -90.0f, 90.0f, "%.1f deg");

        if (changed) {
            float az = glm::radians(_globalLight->_azimuth);
            float el = glm::radians(_globalLight->_elevation);
            _globalLight->_dir = glm::normalize(glm::vec3(
                cos(el) * sin(az),
                sin(el),
                cos(el) * cos(az)
            ));
        }

		ImGui::Text("Light Intensity");
		ImGui::SameLine();
		ImGui::DragFloat("##Light Intensity", &_globalLight->_intensity, 0.0f);

        ImGui::Separator();
        skyboxGUI();
	}

    void GlobalLightGUI::skyboxGUI() {
        auto& skybox = sceneM().activeScene()->getSkybox();

        if (ImGui::CollapsingHeader("Skybox")) {
            for (int i = 0; i < 6; i++) {
                ImGui::Text(Skybox::FACE_NAMES[i]);
                ImGui::SameLine();

                std::string currentTexture = skybox->getFace(i);

                std::string comboId = "##pick" + std::to_string(i);
                if (ImGui::BeginCombo(comboId.c_str(), currentTexture.c_str())) {
                    for (auto& name : rscrM().getAllTextureNames()) {
                        bool is_selected = (currentTexture == name);

                        if (ImGui::Selectable(name.c_str(), is_selected)) {
                            skybox->setFace(i, name);
                        }

                        if (is_selected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
            }

            if (ImGui::Button("Load Skybox")) {
                skybox->load(rscrM().getShader("skybox"));
            }
        }
    }
}
