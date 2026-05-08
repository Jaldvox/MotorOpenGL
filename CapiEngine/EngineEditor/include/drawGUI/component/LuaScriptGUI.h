#pragma once
#include <drawGUI/DrawGUI.h>

namespace cme {
	class LuaScript;
}

namespace cme::editor {
	class LuaScriptGUI : public DrawGUI {
	private:
		LuaScript* _lua;

	public:
		LuaScriptGUI(LuaScript* script);
		void drawOnInspector() override;
	};
}
