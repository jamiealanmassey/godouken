/* godouken_editor_pane.h */

#ifndef GODOUKEN_EDITOR_PANE_H
#define GODOUKEN_EDITOR_PANE_H
#ifdef TOOLS_ENABLED

#include "core/object.h"
#include "scene/gui/button.h"
#include "scene/gui/tool_button.h"
#include "scene/gui/box_container.h"
#include "scene/gui/rich_text_label.h"

#include "godouken_tests/godouken_test_flag.h"

class GodoukenEditorPane : public VBoxContainer {
	GDCLASS(GodoukenEditorPane, VBoxContainer);

private:
	Label *pane_title;
	ToolButton *pane_tool_button;
	RichTextLabel *pane_log;

	Button *pane_button_generate;
	Button *pane_button_clean;
	
#ifdef GODOUKEN_TEST
	Button *pane_button_test;
#endif // GODOUKEN_TEST

	static GodoukenEditorPane *singleton;

private:
#ifdef GODOUKEN_TEST
	void _on_pressed_test();
#endif // GODOUKEN_TEST
	
	void _on_pressed_generate();
	void _on_pressed_clean();

protected:
	static void _bind_methods();

public:
	void setup_elements();
	void set_pane_tool_button(ToolButton *p_tool_button);

	void log_insert(const String &p_message);
	void log_flush();

	RichTextLabel *get_pane_log() const;

	static GodoukenEditorPane *get_singleton();

public:
	GodoukenEditorPane();
	~GodoukenEditorPane();
};

#endif // TOOLS_ENABLED
#endif // GODOUKEN_EDITOR_PANE_H
