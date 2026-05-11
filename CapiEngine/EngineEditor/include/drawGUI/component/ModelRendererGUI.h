#pragma once
#include <drawGUI/DrawGUI.h>
#include <string>

namespace cme {
	class ModelRenderer;
}

namespace cme::editor {
	class ModelRendererGUI : public DrawGUI {
	private:
		cme::ModelRenderer* _meshRenderer;
		std::string _selectedModelName = "";
	public:
		ModelRendererGUI(cme::ModelRenderer* meshRenderer);
		void drawOnInspector() override;
	};
}
