#include "godouken_editor_pane.h"
#include "godouken_tests/godouken_test.h"

#include "godouken_script_translator.h"

#include "editor/editor_node.h"
#include "editor/editor_scale.h"
#include "core/os/file_access.h"
#include "core/class_db.h"

#include "third_party/inja.hpp"

#include "stencils/godouken_stencil_class.h"

GodoukenEditorPane *GodoukenEditorPane::singleton = nullptr;

void GodoukenEditorPane::_on_pressed_test() {
	GodoukenTest *godouken_test = memnew(GodoukenTest);
	godouken_test->execute_tests();
	memdelete(godouken_test);

	nlohmann::json script_result = nlohmann::json::object();
	GodoukenTranslator *godouken_translator = memnew(GodoukenTranslator);
	godouken_translator->evaluate(script_result, "humanoid.gd", "res://assets/entities/_base/");
	script_result["data"]["project_title"] = "";
	script_result["data"]["script"]["breadcrumbs"] = nlohmann::json::array();
	
	/*const String &dir_test_scripts = "res://godouken/test/scripts/";
	DirAccess *dir = DirAccess::create(DirAccess::AccessType::ACCESS_RESOURCES);
	if (!dir->dir_exists(dir_test_scripts)) {
		dir->make_dir_recursive(dir_test_scripts);
	}

	String file_contents = "";
	FileAccess *file = FileAccess::create(FileAccess::AccessType::ACCESS_RESOURCES);
	file->open(dir_test_scripts + "property_test_script1.gd", FileAccess::READ);
	file_contents = file->get_as_utf8_string();
	file->close();
	memdelete(file);*/
	
	inja::Environment env;

	const std::string result = env.render(godouken_stencil_class, script_result);
	const String &dir_scripts = "res://godouken/scripts/";
	DirAccess *dir = DirAccess::create(DirAccess::AccessType::ACCESS_RESOURCES);
	if (!dir->dir_exists(dir_scripts)) {
		dir->make_dir_recursive(dir_scripts);
	}

	FileAccess *file = FileAccess::create(FileAccess::AccessType::ACCESS_RESOURCES);
	file->reopen(dir_scripts + "humanoid.html", FileAccess::WRITE);
	file->store_string(result.c_str());
	file->close();
	memdelete(file);
	memdelete(godouken_translator);
}

void GodoukenEditorPane::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_pressed_test"), &GodoukenEditorPane::_on_pressed_test);
}

void GodoukenEditorPane::setup_elements() {
	VBoxContainer *vb = this;
	HBoxContainer *hb = memnew(HBoxContainer);
	vb->add_child(hb);

	pane_log = memnew(RichTextLabel);
	pane_log->set_scroll_follow(true);
	pane_log->set_selection_enabled(true);
	pane_log->set_focus_mode(FOCUS_CLICK);
	pane_log->set_custom_minimum_size(Size2(0, 180) * EDSCALE);
	pane_log->set_v_size_flags(SIZE_EXPAND_FILL);
	pane_log->set_h_size_flags(SIZE_EXPAND_FILL);

	pane_title = memnew(Label);
	pane_title->set_text(TTR("Godouken:"));
	pane_title->set_h_size_flags(SIZE_EXPAND_FILL);

	pane_button_test = memnew(Button);
	pane_button_test->set_text(TTR("Execute Tests"));
	pane_button_test->connect("pressed", singleton, "_on_pressed_test");

	vb->add_child(pane_log);
	hb->add_child(pane_title);
	hb->add_child(pane_button_test);

	log_insert(TTR("Godouken Documentation Engine : Started successfully"));
	log_insert(TTR(" -- "));
	log_insert(TTR(""));
}

void GodoukenEditorPane::set_pane_tool_button(ToolButton *p_tool_button) {
	pane_tool_button = p_tool_button;
}

void GodoukenEditorPane::log_insert(const String &p_message) {
	pane_log->add_newline();
	pane_log->add_text(p_message);
}

void GodoukenEditorPane::log_flush() {
	pane_log->clear();
}

RichTextLabel *GodoukenEditorPane::get_pane_log() const {
	return nullptr;
}

GodoukenEditorPane *GodoukenEditorPane::get_singleton() {
	if (!singleton) {
		singleton = memnew(GodoukenEditorPane);
	}

	return singleton;
}

GodoukenEditorPane::GodoukenEditorPane() {
	pane_title = nullptr;
	pane_tool_button = nullptr;
	pane_button_test = nullptr;
	pane_log = nullptr;
}

GodoukenEditorPane::~GodoukenEditorPane() {

}
