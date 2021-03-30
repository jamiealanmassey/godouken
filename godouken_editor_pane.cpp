#include "godouken_editor_pane.h"
#include "godouken_tests/godouken_test.h"

#include "godouken_script_translator.h"

#include "editor/editor_node.h"
#include "editor/editor_scale.h"
#include "core/os/file_access.h"
#include "core/class_db.h"

#include "godouken_data_model.h"
#include "godouken_tests/godouken_test_flag.h"

#include "third_party/inja.hpp"

GodoukenEditorPane *GodoukenEditorPane::singleton = nullptr;

#ifdef GODOUKEN_TEST
void GodoukenEditorPane::_on_pressed_test() {
	GodoukenTest *godouken_test = memnew(GodoukenTest);
	godouken_test->execute_tests();
	memdelete(godouken_test);
}
#endif // GODOUKEN_TEST

void GodoukenEditorPane::_on_pressed_generate() {
	nlohmann::json script_result = nlohmann::json::object();
	GodoukenTranslator *godouken_translator = memnew(GodoukenTranslator);
	godouken_translator->evaluate(script_result, "humanoid.gd", "res://assets/entities/_base/");
	script_result["data"]["project_title"] = "";
	script_result["data"]["script"]["breadcrumbs"] = nlohmann::json::array();

	GodoukenDataModel *godouken_data_model = memnew(GodoukenDataModel);
	godouken_data_model->data();
}

void GodoukenEditorPane::_on_pressed_clean() {
	log_insert("-- Clean Documentation Pressed --");
}

void GodoukenEditorPane::_bind_methods() {
#ifdef GODOUKEN_TEST
	ClassDB::bind_method(D_METHOD("_on_pressed_test"), &GodoukenEditorPane::_on_pressed_test);
#endif // GODOUKEN_TEST

	ClassDB::bind_method(D_METHOD("_on_pressed_generate"), &GodoukenEditorPane::_on_pressed_generate);
	ClassDB::bind_method(D_METHOD("_on_pressed_clean"), &GodoukenEditorPane::_on_pressed_clean);
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

#ifdef GODOUKEN_TEST
	pane_button_test = memnew(Button);
	pane_button_test->set_text(TTR("Execute Tests"));
	pane_button_test->connect("pressed", singleton, "_on_pressed_test");
#endif // GODOUKEN_TEST

	pane_button_generate = memnew(Button);
	pane_button_generate->set_text(TTR("Generate Documentation"));
	pane_button_generate->connect("pressed", singleton, "_on_pressed_generate");

	pane_button_clean = memnew(Button);
	pane_button_clean->set_text(TTR("Clean Documentation"));
	pane_button_clean->connect("pressed", singleton, "_on_pressed_clean");
	
	vb->add_child(pane_log);
	hb->add_child(pane_title);

#ifdef GODOUKEN_TEST
	hb->add_child(pane_button_test);
#endif // GODOUKEN_TEST

	hb->add_child(pane_button_generate);
	hb->add_child(pane_button_clean);
	
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
	pane_log = nullptr;
	pane_button_generate = nullptr;
	pane_button_clean = nullptr;
	
#ifdef GODOUKEN_TEST
	pane_button_test = nullptr;
#endif // GODOUKEN_TEST
}

GodoukenEditorPane::~GodoukenEditorPane() {

}
