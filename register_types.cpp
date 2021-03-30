/* register_types.cpp */

#include "register_types.h"

#include "core/class_db.h"
#include "core/engine.h"
#include "core/project_settings.h"
#include "editor/editor_node.h"

#include "godouken.h"
#include "godouken_data_model.h"
#include "godouken_script_translator.h"
#include "godouken_editor_pane.h"

#include "godouken_tests/godouken_test_flag.h"
#include "godouken_tests/godouken_test.h"

GodoukenTranslator *godouken_script_translator = nullptr;

#ifdef TOOLS_ENABLED
static void _initiialise_editor_pane() {
	ToolButton *godouken_pane_tool_button = EditorNode::get_singleton()->add_bottom_panel_item(String("Godouken"), GodoukenEditorPane::get_singleton());
	GodoukenEditorPane::get_singleton()->setup_elements();
	GodoukenEditorPane::get_singleton()->set_pane_tool_button(godouken_pane_tool_button);
	Engine::get_singleton()->add_singleton(Engine::Singleton("GodoukenPane", GodoukenEditorPane::get_singleton()));
}
#endif

void register_godouken_types() {
	ClassDB::register_class<Godouken>();
	ClassDB::register_class<GodoukenDataModel>();
	ClassDB::register_class<GodoukenTranslator>();

#ifdef GODOUKEN_TEST
	ClassDB::register_class<GodoukenTest>();
#endif // GODOUKEN_TEST

#ifdef TOOLS_ENABLED
	ClassDB::set_current_api(ClassDB::API_EDITOR);
	ClassDB::register_class<GodoukenEditorPane>();
	ClassDB::set_current_api(ClassDB::API_CORE);
	EditorNode::add_plugin_init_callback(_initiialise_editor_pane);
#endif

	if (!ProjectSettings::get_singleton()->has_setting("godouken/config/website")) {
		ProjectSettings::get_singleton()->set_setting("godouken/config/website", "");
	}

	if (!ProjectSettings::get_singleton()->has_setting("godouken/config/include_reference")) {
		ProjectSettings::get_singleton()->set_setting("godouken/config/include_reference", true);
	}

	if (!ProjectSettings::get_singleton()->has_setting("godouken/config/include_console_suggestions")) {
		ProjectSettings::get_singleton()->set_setting("godouken/config/include_console_suggestions", true);
	}

	if (!ProjectSettings::get_singleton()->has_setting("godouken/config/exclude_directories")) {
		PoolStringArray exclude;
		exclude.push_back("res://addons");
		ProjectSettings::get_singleton()->set_setting("godouken/config/exclude_directories", exclude);
	}
}

void unregister_godouken_types() {
	// empty
}
