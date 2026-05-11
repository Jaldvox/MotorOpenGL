#include <drawGUI/component/ModelRendererGUI.h>
#include <component/ModelRenderer.h>
#include <mesh/Model.h>
#include <surface/Shader.h>
#include <imgui.h>
#include <managers/ResourceManager.h>

namespace cme::editor {
	ModelRendererGUI::ModelRendererGUI(cme::ModelRenderer* meshRenderer) : _meshRenderer(meshRenderer) {}

	void ModelRendererGUI::drawOnInspector() {
		if (!ImGui::CollapsingHeader("Model Renderer")) return;
		auto model = _meshRenderer->model();

		if (ImGui::BeginCombo("##combo", _selectedModelName.c_str())) {
			for (auto& m : rscrM().getAllModelNames()) {
				bool isSelected = _selectedModelName == m;
				if (ImGui::Selectable(m.c_str(), isSelected)) {
					_meshRenderer->setModel(rscrM().getModel(m));
					_selectedModelName = m;
				}
			}
			ImGui::EndCombo();
		}

		if (!model) return;

		ImGui::Text("Path: %s", model->path().c_str());

		if (model) {
			ImGui::Text("Sub-meshes: %zu", model->subMeshes().size());

			glm::vec3 bmin, bmax;
			model->getLocalAABB(bmin, bmax);
			ImGui::Text("AABB min: (%.2f, %.2f, %.2f)", bmin.x, bmin.y, bmin.z);
			ImGui::Text("AABB max: (%.2f, %.2f, %.2f)", bmax.x, bmax.y, bmax.z);
		}

		// Input de ruta + botón de carga
		static char pathBuf[512] = {};
		if (model->path().size() < sizeof(pathBuf))
			std::copy(model->path().begin(), model->path().end(), pathBuf);

		// Inspector por sub-mesh (shader + propiedades del material)
		int idx = 0;
		for (auto& sub : model->subMeshes()) {
			std::string label = "Sub-mesh " + std::to_string(idx++);
			if (ImGui::TreeNode(label.c_str())) {
				if (sub.material) {
					Shader* sh = sub.material->getShader();
					ImGui::Text("Shader: %s", sh ? sh->getName().c_str() : "(null)");
					ImGui::Text("Texturas: %zu", sub.ownedTextures.size());
				}
				ImGui::TreePop();
			}
		}
	}
}