/* godouken_script_translator.cpp */

#include "godouken_script_translator.h"
#include "godouken_data_model.h"

#include "third_party/inja.hpp"
#include "third_party/crc.h"

/// GodoukenScriptTranslatorMemberData
GodoukenScriptTranslatorMemberData::GodoukenScriptTranslatorMemberData() {}
GodoukenScriptTranslatorMemberData::~GodoukenScriptTranslatorMemberData() {}

/// GodoukenScriptTranslatorPropertyData
GodoukenScriptTranslatorPropertyData::GodoukenScriptTranslatorPropertyData() {}
GodoukenScriptTranslatorPropertyData::~GodoukenScriptTranslatorPropertyData() {}

/// GodoukenScriptTranslatorMethodData
GodoukenScriptTranslatorMethodData::GodoukenScriptTranslatorMethodData() {}
GodoukenScriptTranslatorMethodData::~GodoukenScriptTranslatorMethodData() {}

/// GodoukenScriptTranslatorCommentParser
GodoukenScriptTranslatorCommentData::GodoukenScriptTranslatorCommentData() {
	comment_body = "";
	comment_selector = "";
}

GodoukenScriptTranslatorCommentData::GodoukenScriptTranslatorCommentData(const GodoukenScriptTranslatorCommentData &p_comment_data) {
	comment_body = p_comment_data.comment_body;
	comment_selector = p_comment_data.comment_selector;
}

GodoukenScriptTranslatorCommentData::GodoukenScriptTranslatorCommentData(const String &p_comment_body, const String &p_comment_selector) {
	comment_body = p_comment_body;
	comment_selector = p_comment_selector;
}

void GodoukenScriptTranslatorCommentParser::parse(const Vector<String> &p_lines, const int32_t &p_from, const int32_t &p_to) {
	int index = 0;
	for (int32_t i = 0; i < p_lines.size(); i++) {
		if (index >= p_from) {
			String comment_keyword = "";
			String comment_content = "";
			String comment_selector = "";

			const String &line = p_lines[i];
			const int32_t find_at = line.find("@");
			const int32_t find_sep = line.find(":", find_at);
			if (find_at > 0 && find_sep > 0) {
				comment_keyword = line.substr(find_at, find_sep - find_at).strip_edges();
				comment_content = line.substr(find_sep + 1).strip_edges();

				const int32_t brace_0 = line.find_char('[', find_at);
				const int32_t brace_1 = line.find_char(']', brace_0);
				if (brace_0 > 0 && brace_1 > 0) {
					comment_selector = line.substr(brace_1 - brace_0).strip_edges();
				}

				GodoukenScriptTranslatorCommentData *comment_data = memnew(GodoukenScriptTranslatorCommentData);
				comment_data->comment_body = comment_content;
				comment_data->comment_selector = comment_selector;

				if (!comment_entries.has(comment_keyword)) {
					comment_entries[comment_keyword] = CommentDataSet();
				}

				comment_entries[comment_keyword].push_back(comment_data);
			}
		}

		index++;
		if (index >= p_to) {
			break;
		}
	}
}

void GodoukenScriptTranslatorCommentParser::parse(const Vector<String> &p_lines) {
	parse(p_lines, 0, p_lines.size());
}

void GodoukenScriptTranslatorCommentParser::flush() {
	comment_entries.clear();
}

const CommentDataStore &GodoukenScriptTranslatorCommentParser::get_data_store() const {
	return comment_entries;
}

const CommentDataSet &GodoukenScriptTranslatorCommentParser::get_data_set(const String &p_key) const {
	if (comment_entries.has(p_key)) {
		return comment_entries[p_key];
	}

	return CommentDataSet_0;
}

const bool GodoukenScriptTranslatorCommentParser::has_data_set(const String &p_key) const {
	return comment_entries.has(p_key);
}

const GodoukenScriptTranslatorCommentData *GodoukenScriptTranslatorCommentParser::get_data_set_first(const String &p_key/*, const bool p_remove_element*/) {
	GodoukenScriptTranslatorCommentData *result = memnew(GodoukenScriptTranslatorCommentData);
	if (comment_entries.has(p_key)) {
		const CommentDataSet &comment_data_set = comment_entries[p_key];
		if (comment_data_set.size() > 0) {
			memdelete(result);
			result = comment_data_set[0];
		}
	}

	return result;
}

GodoukenScriptTranslatorCommentParser::GodoukenScriptTranslatorCommentParser() {}
GodoukenScriptTranslatorCommentParser::~GodoukenScriptTranslatorCommentParser() {}

/// GodoukenScriptTranslator
/*const Dictionary GodoukenScriptTranslator::get_members_to_line(const Ref<Script> &p_script) {
    Dictionary members_to_line = Dictionary();
	return members_to_line;
}

const Array &GodoukenScriptTranslator::get_sorted_keys(Array &p_keys) {
	if (p_keys.size() <= 1) {
		return p_keys;
	}

	Array keys_sorted = Array();
	Variant *keys_pure = new Variant[p_keys.size()];
	for (int32_t i = 0; i < p_keys.size(); i++) {
		int index = translator_script->get_member_line(keys_pure[i]);
		int j = i;

		while (j > 0 && translator_script->get_member_line(keys_pure[i - 1]) > index) {
			keys_pure[j] = keys_pure[j - 1];
			j--;
		}

		keys_pure[j] = index;
	}

	for (int32_t i = 0; i < p_keys.size(); i++) {
		keys_sorted.push_back(Variant(keys_pure[i]));
	}

	p_keys = keys_sorted;
	return p_keys;
}

const List<String> GodoukenScriptTranslator::get_script_lines(const FileAccess *p_script_file) {
	List<String> script_lines;
	String script_line = p_script_file->get_line();
	while (!(script_line = p_script_file->get_line()).empty()) {
		script_lines.push_back(script_line);
	}

	return script_lines;
}

const int32_t GodoukenScriptTranslator::get_script_line_begin(const List<String> &p_script_lines) {
	int32_t index = 0;
	for (const List<String>::Element *E = p_script_lines.front(); E; E = E->next()) {
		if (E->get().find("extends") >= 0) {
			return index;
		}
	}

	return -1;
}

const GodoukenDataNodeMeta GodoukenScriptTranslator::get_meta_data() const {
    GodoukenDataNodeMeta temp(0.0f, 0.0f, "", "", "");
    return temp;
}

void GodoukenScriptTranslator::evaluate(const String &p_member_name) {
	if (script_members_to_line.has(p_member_name)) {
		Object *script_member_obj = script_members_to_line[p_member_name];
		GodoukenScriptTranslatorMemberData *script_member_data = (GodoukenScriptTranslatorMemberData *)script_member_obj;
		GodoukenScriptTranslatorCommentParser *comment_data_parser = memnew(GodoukenScriptTranslatorCommentParser);
		translator_script_line_begin = translator_script->get_member_line(p_member_name);
		comment_data_parser->parse(translator_script_lines, translator_script_line_begin, translator_script_line_end);

		switch (script_member_data->member_type) {
			case 0:
				evaluate_property(p_member_name, comment_data_parser);
				break;
			case 1:
				evaluate_method(p_member_name, comment_data_parser);
				break;
			case 2:
				evaluate_signal(p_member_name, comment_data_parser);
				break;
			default:
				break;
		}

		memdelete(comment_data_parser);
		translator_script_line_begin = translator_script_line_end;
	}
}

void GodoukenScriptTranslator::evaluate_property(const String& p_member_name, GodoukenScriptTranslatorCommentParser* p_comment_parser) {
	if (!script_property_infos.has(p_member_name)) {
		return;
	}

	const PropertyInfo &property_info = script_property_infos[p_member_name];
	GodoukenDataNodeProperty property_data;
	property_data.node_name = property_info.name;
	property_data.node_collection = p_comment_parser->get_data_set_first("collection")->comment_body;
	property_data.node_desc_brief = p_comment_parser->get_data_set_first("brief")->comment_body;
	property_data.node_desc_detailed = p_comment_parser->get_data_set_first("detailed")->comment_body;
	property_data.node_extra_warning = p_comment_parser->get_data_set_first("warning")->comment_body;
	property_data.node_extra_info = p_comment_parser->get_data_set_first("info")->comment_body;
	property_data.node_return_type = GodoukenDataType(property_info.type != Variant::Type::OBJECT, property_info.class_name);
	script_data_properties[property_info.name] = property_data;
}

void GodoukenScriptTranslator::evaluate_method(const String &p_member_name, GodoukenScriptTranslatorCommentParser *p_comment_parser) {
	if (!script_method_infos.has(p_member_name)) {
		return;
	}

	const MethodInfo &method_info = script_method_infos[p_member_name];
	GodoukenDataNodeMethod method_data;
	method_data.method_is_godot = reserved_godot_methods.find(method_info.name);
	method_data.method_is_internal = method_info.name[0] == '_';
	method_data.node_name = method_info.name;
	method_data.node_collection = p_comment_parser->get_data_set_first("collection")->comment_body;
	method_data.node_desc_brief = p_comment_parser->get_data_set_first("brief")->comment_body;
	method_data.node_desc_detailed = p_comment_parser->get_data_set_first("detailed")->comment_body;
	method_data.node_extra_warning = p_comment_parser->get_data_set_first("warning")->comment_body;
	method_data.node_extra_info = p_comment_parser->get_data_set_first("info")->comment_body;
	method_data.method_return_msg = p_comment_parser->get_data_set_first("return")->comment_body;
	method_data.method_see_also = p_comment_parser->get_data_set_first("see")->comment_body.split(",");
	method_data.node_return_type = GodoukenDataType(false, method_info.return_val.class_name);

	const CommentDataSet &parameter_data_set = p_comment_parser->get_data_set("parameter");
	for (int32_t i = 0; i < parameter_data_set.size(); i++) {
		const GodoukenScriptTranslatorCommentData *parameter_data = parameter_data_set[i];
		if (parameter_data) {
			GodoukenDataType parameter_type;
			parameter_type.type_is_godot = false;
			parameter_type.type_name = "_";

			for (int32_t i = 0; i < method_info.arguments.size(); i++) {
				const PropertyInfo &argument = method_info.arguments[i];
				if (argument.name.find(parameter_data->comment_selector) == 0) {
					parameter_type.type_is_godot = argument.type != Variant::Type::OBJECT;
					parameter_type.type_name = argument.class_name;
				}
			}

			GodoukenDataParameter parameter;
			parameter.param_identifier = parameter_data->comment_selector;
			parameter.param_desc = parameter_data->comment_body;
			parameter.param_type_data = parameter_type;
			method_data.method_params.push_back(parameter);
		}
	}

	const CommentDataSet &link_data_set = p_comment_parser->get_data_set("link");
	for (int32_t i = 0; i < link_data_set.size(); i++) {
		const GodoukenScriptTranslatorCommentData *link_data = link_data_set[i];
		if (link_data) {
			GodoukenDataMetaLink meta_link;
			meta_link.link_label = link_data->comment_selector;
			meta_link.link_address = link_data->comment_body;
			method_data.method_links.push_back(meta_link);
		}
	}

	const CommentDataSet &todo_data_set = p_comment_parser->get_data_set("todo");
	for (int32_t i = 0; i < todo_data_set.size(); i++) {
		const GodoukenScriptTranslatorCommentData *todo_data = todo_data_set[i];
		if (todo_data) {
			GodoukenDataMetaTodo meta_todo;
			meta_todo.todo_desc = todo_data->comment_body;
			meta_todo.todo_owner = todo_data->comment_selector;
			method_data.method_todos.push_back(meta_todo);
		}
	}

	script_data_methods[method_info.name] = method_data;
}

void GodoukenScriptTranslator::evaluate_signal(const String &p_member_name, GodoukenScriptTranslatorCommentParser *p_comment_parser) {
	if (!script_signal_infos.has(p_member_name)) {
		return;
	}

	const MethodInfo &signal_info = script_method_infos[p_member_name];
	GodoukenDataNodeSignal signal_data;
	signal_data.node_name = signal_info.name;
	signal_data.node_desc_brief = p_comment_parser->get_data_set_first("brief")->comment_body;
	signal_data.node_desc_detailed = p_comment_parser->get_data_set_first("detailed")->comment_body;

	const CommentDataSet &parameter_data_set = p_comment_parser->get_data_set("parameter");
	for (int32_t i = 0; i < parameter_data_set.size(); i++) {
		const GodoukenScriptTranslatorCommentData *parameter_data = parameter_data_set[i];
		if (parameter_data) {
			GodoukenDataType parameter_type;
			parameter_type.type_is_godot = false;
			parameter_type.type_name = "_";

			for (int32_t i = 0; i < signal_info.arguments.size(); i++) {
				const PropertyInfo &argument = signal_info.arguments[i];
				if (argument.name.find(parameter_data->comment_selector) == 0) {
					parameter_type.type_is_godot = argument.type != Variant::Type::OBJECT;
					parameter_type.type_name = argument.class_name;
				}
			}

			GodoukenDataParameter parameter;
			parameter.param_identifier = parameter_data->comment_selector;
			parameter.param_desc = parameter_data->comment_body;
			parameter.param_type_data = parameter_type;
			signal_data.signal_params.push_back(parameter);
		}
	}

	script_data_signals[signal_info.name] = signal_data;
}

GodoukenDataModel *GodoukenScriptTranslator::script_translate(const String &p_script_name, const String &p_script_directory) { 
    Error *translator_script_status = nullptr;
	FileAccess *translator_script_file = nullptr;
	GodoukenDataModel *translator_data_model = nullptr;

	translator_script_file = FileAccess::open(p_script_directory + p_script_name, FileAccess::READ, translator_script_status);
	if (translator_script_status && (*translator_script_status) == Error::OK) {
		translator_script->set_source_code(translator_script_file->get_as_utf8_string());

		if (translator_script->is_valid()) {
			translator_data_model = memnew(GodoukenDataModel);
			List<MethodInfo> *translator_script_methods = nullptr;
			List<MethodInfo> *translator_script_signals = nullptr;
			List<PropertyInfo> *translator_script_properties = nullptr;

			translator_script->get_script_method_list(translator_script_methods);
			translator_script->get_script_signal_list(translator_script_signals);
			translator_script->get_script_property_list(translator_script_properties);

			int32_t index = 0;
			for (index = 0; index < (*translator_script_properties).size(); index++) {
				const PropertyInfo &property_info = (*translator_script_properties)[index];
				const String &property_name = property_info.name;

				GodoukenScriptTranslatorMemberData *member_data = memnew(GodoukenScriptTranslatorMemberData);
				member_data->member_type = 0;
				member_data->member_line = translator_script->get_member_line(property_name);
				script_members_to_line[property_name] = member_data;
				script_property_infos[property_name] = property_info;
			}

			for (index = 0; index < (*translator_script_methods).size(); index++) {
				const MethodInfo &method_info = (*translator_script_methods)[index];
				const String &method_name = method_info.name;

				GodoukenScriptTranslatorMemberData *member_data = memnew(GodoukenScriptTranslatorMemberData);
				member_data->member_type = 1;
				member_data->member_line = translator_script->get_member_line(method_name);
				script_members_to_line[method_name] = member_data;
				script_method_infos[method_name] = method_info;
			}

			for (index = 0; index < (*translator_script_signals).size(); index++) {
				const MethodInfo &signal_info = (*translator_script_signals)[index];
				const String &signal_name = signal_info.name;

				GodoukenScriptTranslatorMemberData *member_data = memnew(GodoukenScriptTranslatorMemberData);
				member_data->member_type = 2;
				member_data->member_line = translator_script->get_member_line(signal_name);
				script_members_to_line[signal_name] = member_data;
				script_signal_infos[signal_name] = signal_info;
			}

			const Array &members_to_keys = get_sorted_keys(script_members_to_line.keys());
			translator_script_lines = get_script_lines(translator_script_file);
			translator_script_line_begin = get_script_line_begin(translator_script_lines);

			if (translator_script_line_begin >= 0 && !members_to_keys.empty()) {
				GodoukenDataNodeMeta meta_data;
				GodoukenScriptTranslatorCommentParser *comment_parser = memnew(GodoukenScriptTranslatorCommentParser);
				comment_parser->parse(translator_script_lines, 0, translator_script_line_begin);
				meta_data.meta_collection = comment_parser->get_data_set_first("collection")->comment_body;
				meta_data.meta_created = comment_parser->get_data_set_first("created")->comment_body;
				meta_data.meta_version = comment_parser->get_data_set_first("version")->comment_body.to_float();
				meta_data.meta_modified = comment_parser->get_data_set_first("modified")->comment_body;
				meta_data.meta_deprecated = comment_parser->get_data_set_first("deprecated")->comment_body.to_float();

				for (int32_t i = 0; i < members_to_keys.size(); i++) {
					evaluate(members_to_keys[i]);
				}

				GodoukenData *root_node = memnew(GodoukenData);
				translator_data_model = memnew(GodoukenDataModel);
				translator_data_model->node_index_insert(root_node);
				root_node->node_children.push_back((GodoukenData *) &meta_data);

				GodoukenData *root_node_methods = memnew(GodoukenData);
				GodoukenData *root_node_signals = memnew(GodoukenData);
				GodoukenData *root_node_properties = memnew(GodoukenData);

				for (const Map<String, GodoukenDataNodeMethod>::Element *E = script_data_methods.front(); E; E->next()) {
					const GodoukenDataNodeMethod &node_method = E->value();
					root_node_methods->node_children.push_back((GodoukenData *)&node_method);
				}

				for (const Map<String, GodoukenDataNodeSignal>::Element *E = script_data_signals.front(); E; E->next()) {
					const GodoukenDataNodeSignal &node_signal = E->value();
					root_node_signals->node_children.push_back((GodoukenData *)&node_signal);
				}

				for (const Map<String, GodoukenDataNodeProperty>::Element *E = script_data_properties.front(); E; E->next()) {
					const GodoukenDataNodeProperty &node_property = E->value();
					root_node_properties->node_children.push_back((GodoukenData *)&node_property);
				}

				root_node->node_children.push_back(root_node_methods);
				root_node->node_children.push_back(root_node_signals);
				root_node->node_children.push_back(root_node_properties);
			}
		}
	}

    return translator_data_model;
}

#include "stencils/godouken_stencil_class.h"
#include "stencils/godouken_stencil_bootstrap.h"
#include "stencils/godouken_stencil_class_navbar.h"
#include "stencils/godouken_stencil_style_shared.h"
#include "stencils/godouken_stencil_style_class.h"

GodoukenScriptTranslator::GodoukenScriptTranslator() :
    translator_script(nullptr),
    translator_script_line_begin(-1),
    translator_script_line_end(-1) {
    reserved_godot_methods.push_back("_init");
    reserved_godot_methods.push_back("_ready");
    reserved_godot_methods.push_back("_process");
    reserved_godot_methods.push_back("_process_physics");
    reserved_godot_methods.push_back("_enter_tree");
    reserved_godot_methods.push_back("_exit_tree");
    reserved_godot_types.push_back("bool");
    reserved_godot_types.push_back("int");
    reserved_godot_types.push_back("float");
    reserved_godot_types.push_back("String");
    reserved_godot_types.push_back("Vector2");
    reserved_godot_types.push_back("Vector3");
    reserved_godot_types.push_back("Rect2");
    reserved_godot_types.push_back("Transform2D");
    reserved_godot_types.push_back("Transform");
    reserved_godot_types.push_back("Plane");
    reserved_godot_types.push_back("Quat");
    reserved_godot_types.push_back("AABB");
    reserved_godot_types.push_back("Basis");
    reserved_godot_types.push_back("Color");
    reserved_godot_types.push_back("NodePath");
    reserved_godot_types.push_back("RID");
    reserved_godot_types.push_back("Object");
    reserved_godot_types.push_back("Array");
    reserved_godot_types.push_back("Dictionary");
    reserved_godot_types.push_back("PoolByteArray");
    reserved_godot_types.push_back("PoolIntArray");
    reserved_godot_types.push_back("PoolRealArray");
    reserved_godot_types.push_back("PoolStringArray");
    reserved_godot_types.push_back("PoolVector2Array");
    reserved_godot_types.push_back("PoolVector3Array");
    reserved_godot_types.push_back("PoolColorArray");

	nlohmann::json script;
	nlohmann::json script_properties;
	nlohmann::json script_properties_test1;
	nlohmann::json script_properties_test2;
	nlohmann::json styling_settings;

	script_properties_test1["name"] = "Property1";
	script_properties_test1["description"]["brief"] = "This is a short description";
	script_properties_test1["description"]["detailed"] = "This is a detailed description";
	script_properties_test1["type_info"]["name"] = "int";
	script_properties_test1["type_info"]["href"] = "https://docs.godotengine.org/en/3.2/classes/class_int.html";
	script_properties_test1["tags"]["is_godot"] = true;
	script_properties_test1["tags"]["is_exported"] = false;

	script_properties_test2["name"] = "Property2";
	script_properties_test2["description"]["brief"] = "This is a short description";
	script_properties_test2["description"]["detailed"] = "This is a detailed description";
	script_properties_test2["type_info"]["name"] = "void";
	script_properties_test2["type_info"]["href"] = nullptr;
	script_properties_test2["tags"]["is_godot"] = false;
	script_properties_test2["tags"]["is_exported"] = false;

	script["data"]["project_title"] = "Test Project";
	script["data"]["script"]["name"] = "TestScript";
	script["data"]["script"]["meta"]["deprecated"] = nullptr;
	script["data"]["script"]["meta"]["version"] = 1.0f;
	script["data"]["script"]["meta"]["created"] = "04/02/2021";
	script["data"]["script"]["meta"]["modified"] = nullptr;
	script["data"]["script"]["meta"]["collection"] = nullptr;
	script["data"]["script"]["description"]["brief"] = "This is a short description";
	script["data"]["script"]["description"]["detailed"] = "This is a detailed description";

	script["data"]["script"]["breadcrumbs"] = nlohmann::json::array();
	script["data"]["script"]["breadcrumbs"].push_back("assets");
	script["data"]["script"]["breadcrumbs"].push_back("entities");
	script["data"]["script"]["breadcrumbs"].push_back("_base");
	script["data"]["script"]["breadcrumbs"].push_back("TestScript.gd");

	script["data"]["script"]["properties"] = nlohmann::json::array();
	script["data"]["script"]["properties"].push_back(script_properties_test1);
	script["data"]["script"]["properties"].push_back(script_properties_test2);

	styling_settings["styling"]["colours"]["background"]["white"] = "#FFFFFF";
	styling_settings["styling"]["colours"]["background"]["blue"] = "#179AEB";
	styling_settings["styling"]["colours"]["background"]["red"] = "#C21D1D";
	styling_settings["styling"]["colours"]["background"]["green"] = "#1DC254";
	styling_settings["styling"]["colours"]["background"]["yellow"] = "#E3E34B";
	styling_settings["styling"]["colours"]["background"]["orange"] = "#EBA626";

	styling_settings["styling"]["colours"]["text"]["white"] = "#FFFFFF";
	styling_settings["styling"]["colours"]["text"]["blue"] = "#179AEB";
	styling_settings["styling"]["colours"]["text"]["red"] = "#C21D1D";
	styling_settings["styling"]["colours"]["text"]["green"] = "#1DC254";
	styling_settings["styling"]["colours"]["text"]["yellow"] = "#E3E34B";
	styling_settings["styling"]["colours"]["text"]["orange"] = "#EBA626";

	inja::Environment environment_html { "./godouken/html/" };
	inja::Environment environment_css { "./godouken/css/" };
	inja::Environment environment_js{ "./godouken/js/" };

	inja::Template template_html_class = environment_html.parse(godouken_stencil_class);
	inja::Template template_html_class_navbar = environment_html.parse(godouken_stencil_class_navbar);
	inja::Template template_css_shared = environment_css.parse(godouken_stencil_style_shared);
	inja::Template template_css_class = environment_css.parse(godouken_stencil_style_class);

	environment_html.write(template_html_class, script, "class_test.html");
	environment_html.write(template_html_class_navbar, nullptr, "class_navigation.html");
	environment_css.write(template_css_shared, styling_settings, "style_shared.css");
	environment_css.write(template_css_class, nullptr, "style_class.css");
}

GodoukenScriptTranslator::~GodoukenScriptTranslator() {
    if (translator_script) {
        memdelete(translator_script);
    }

    translator_script_lines.clear();
	translator_script_lines.clear();
    reserved_godot_methods.clear();
    reserved_godot_types.clear();
}*/






const Dictionary GodoukenTranslatorV2::get_members_to_line(const Ref<Script> &p_script) {
	Dictionary members_to_line = Dictionary();
	return members_to_line;
}

const Array &GodoukenTranslatorV2::get_sorted_keys(Array &p_keys) {
	if (p_keys.size() <= 1) {
		return p_keys;
	}

	Array keys_sorted = Array(p_keys);
	// Variant *keys_pure = new Variant[p_keys.size()];
	/*for (int32_t i = 1; i < p_keys.size(); i++) {
		int line = script->get_member_line(p_keys[i]);
		int line_prev = script->get_member_line(p_keys[i - 1]);
		int j = i;

		while (j > 0 && line_prev > script->get_member_line(p_keys[j])) {
			keys_sorted[j] = keys_sorted[j - 1];
			j--;
		}

		keys_sorted[j] = keys_sorted[i];
	}*/

	/*for (int32_t i = 0; i < p_keys.size(); i++) {
		keys_sorted.push_back(Variant(keys_pure[i]));
	}*/

	p_keys = keys_sorted;
	return p_keys;
}

const List<String> GodoukenTranslatorV2::get_script_lines(const FileAccess *p_script_file) {
	List<String> script_lines;
	String script_line = p_script_file->get_line();
	while (!(script_line = p_script_file->get_line()).empty()) {
		script_lines.push_back(script_line);
	}

	return script_lines;
}

const int32_t GodoukenTranslatorV2::get_script_line_begin(const Vector<String> &p_script_lines) {
	int32_t index = -1;
	for (int32_t i = 0; i < p_script_lines.size(); i++) {
		if (p_script_lines[i].find("extends") >= 0) {
			index = i;
			break;
		}
	}

	return index;
}

void GodoukenTranslatorV2::evaluate_member(const String& p_member_name) {
	if (script_members_to_line.has(p_member_name)) {
		script_line_begin = script->get_member_line(p_member_name);

		Object *script_member_obj = script_members_to_line[p_member_name];
		GodoukenScriptTranslatorMemberData *script_member_data = (GodoukenScriptTranslatorMemberData *)script_member_obj;
		if (script_member_data->member_type == 0) {

		} else if (script_member_data->member_type == 1) {
			/*MethodInfo method_info = script->get_method_info(p_member_name);
			GodoukenDataNodeMethod method_data;
			GodoukenScriptTranslatorCommentParser *comment_data_parser = memnew(GodoukenScriptTranslatorCommentParser);
			comment_data_parser->parse(script_lines, script_line_begin, script_line_finish);
			method_data.method_is_godot = script_reserved_godot_methods.find(method_info.name);
			method_data.method_is_internal = method_info.name[0] == '_';
			method_data.node_name = method_info.name;
			method_data.node_desc_brief = comment_data_parser->get_p("brief", true);
			method_data.node_desc_detailed = comment_data_parser->get_parsed_entry("detailed", true);
			method_data.node_extra_warning = comment_data_parser->get_parsed_entry("warning", true);
			method_data.node_extra_info = comment_data_parser->get_parsed_entry("info", true);
			method_data.method_return_msg = comment_data_parser->get_parsed_entry("return", true);
			method_data.method_see_also = comment_data_parser->get_parsed_entry("see", true).split(",");
			method_data.node_return_type = GodoukenDataType(false, method_info.return_val.class_name);

			const Dictionary &comment_entries = comment_data_parser->get_comment_entires();
			const Array &comment_keys = comment_entries.keys();
			for (int32_t i = 0; i < comment_keys.size(); i++) {
				const String &comment_key = comment_keys[i];
				const String &comment_val = comment_entries[comment_key];
				if (comment_key.find("param") > 0) {
					const int32_t brace_0 = comment_key.find_char('[');
					const int32_t brace_1 = comment_key.find_char(']');
					if (brace_0 >= 0 && brace_1 >= 0 && brace_0 != brace_1) {
						GodoukenDataParameter method_parameter;
						const List<PropertyInfo> method_args = method_info.arguments;
						const String &property_name = comment_key.substr(brace_0, brace_1 - brace_0);
						method_parameter.param_identifier = property_name;
						method_parameter.param_desc = comment_val;

						for (int32_t i = 0; i < method_args.size(); i++) {
							if (method_args[i].name.find(property_name) == 0) {
								const PropertyInfo &property_info = method_args[i];
								const String &property_type = String(property_info.class_name);
								method_parameter.param_type_data = GodoukenDataType(script_reserved_godot_types.find(property_type), property_type);
							}
						}

						method_data.method_params.push_back(method_parameter);
					}
				}
			}*/
		} else if (script_member_data->member_type == 2) {
		}

		script_line_finish = script_line_begin;
	}
}

void GodoukenTranslatorV2::evaluate_signal(nlohmann::json &p_script_json, const MethodInfo &p_signal_info) {
	
}

void GodoukenTranslatorV2::evaluate_method(nlohmann::json &p_script_json, const MethodInfo &p_method_info) {
	
}

void GodoukenTranslatorV2::evaluate_property(nlohmann::json &p_script_json, const PropertyInfo &p_property_info) {
	GodoukenScriptTranslatorCommentParser *comment_parser = memnew(GodoukenScriptTranslatorCommentParser);
	comment_parser->parse(script_lines, script_line_begin, script_line_finish);

	nlohmann::json property_json = nlohmann::json::object();
	property_json["name"] = p_property_info.name.utf8();
	property_json["description"]["brief"] = comment_parser->get_data_set_first("@brief")->comment_body.utf8();
	property_json["description"]["detailed"] = comment_parser->get_data_set_first("@detailed")->comment_body.utf8();
	property_json["type_info"]["name"] = ""; //p_property_info.name.utf8();
	property_json["type_info"]["href"] = script_type_documentation.has(p_property_info.type) ? script_type_documentation[p_property_info.type].utf8() : "";
	property_json["tags"]["is_godot"] = (p_property_info.type != Variant::NIL && p_property_info.type != Variant::OBJECT);
	property_json["tags"]["is_exported"] = (p_property_info.usage & PROPERTY_USAGE_EDITOR) == PROPERTY_USAGE_EDITOR;
	p_script_json["script"]["properties"].push_back(property_json);
}

void GodoukenTranslatorV2::evaluate_script(nlohmann::json &p_script_json, const Array p_members_to_keys) {
	//nlohmann::json script_json = nlohmann::json::object();
	for (int32_t i = 0; i < p_members_to_keys.size(); i++) {
		const String &member_name = p_members_to_keys.get(i);
		Object *member_data_ptr = script_members_to_line[member_name];
		GodoukenScriptTranslatorMemberData *member_data = (GodoukenScriptTranslatorMemberData *)member_data_ptr;
		if (member_data) {
			script_line_finish = member_data->member_line;
			switch (member_data->member_type) {
				case 0:
					//script_json["properties"].push_back(evaluate_property(((GodoukenScriptTranslatorPropertyData *)member_data)->member_property_info));
					evaluate_property(p_script_json, ((GodoukenScriptTranslatorPropertyData *)member_data)->member_property_info);
					break;
				case 1:
					// script_json["script"]["methods"].push_back(evaluate_method(((GodoukenScriptTranslatorMethodData *)member_data)->member_method_info));
					break;
				case 2:
					// script_json["script"]["signals"].push_back(evaluate_signal(((GodoukenScriptTranslatorMethodData *)member_data)->member_method_info));
					break;
				default:
					break;
			}
		}

		script_line_begin = script_line_finish;
	}
}

void GodoukenTranslatorV2::evaluate(nlohmann::json &p_script_json, const String &p_code) {
	//nlohmann::json script_json = nlohmann::json::object();
	script = memnew(GDScript);
	script->set_source_code(p_code);
	script->reload(true);
	if (script->is_valid()) {
		List<MethodInfo> *script_methods = new List<MethodInfo>();
		List<MethodInfo> *script_signals = new List<MethodInfo>();
		List<PropertyInfo> *script_properties = new List<PropertyInfo>();

		script->get_script_method_list(script_methods);
		script->get_script_signal_list(script_signals);
		script->get_script_property_list(script_properties);

		int32_t index = 0;
		if (script_properties) {
			for (index = 0; index < script_properties->size(); index++) {
				const PropertyInfo &property_info = (*script_properties)[index];
				const String &property_name = property_info.name;

				GodoukenScriptTranslatorPropertyData *member_data = memnew(GodoukenScriptTranslatorPropertyData);
				member_data->member_type = 0;
				member_data->member_line = script->get_member_line(property_name);
				member_data->member_property_info = property_info;
				script_members_to_line[property_name] = member_data;
			}
		}

		if (script_methods) {
			for (index = 0; index < script_methods->size(); index++) {
				const MethodInfo &method_info = (*script_methods)[index];
				const String &method_name = method_info.name;

				GodoukenScriptTranslatorMethodData *member_data = memnew(GodoukenScriptTranslatorMethodData);
				member_data->member_type = 1;
				member_data->member_line = script->get_member_line(method_name);
				member_data->member_method_info = method_info;
				script_members_to_line[method_name] = member_data;
			}
		}

		if (script_signals) {
			for (index = 0; index < script_signals->size(); index++) {
				const MethodInfo &signal_info = (*script_signals)[index];
				const String &signal_name = signal_info.name;

				GodoukenScriptTranslatorMethodData *member_data = memnew(GodoukenScriptTranslatorMethodData);
				member_data->member_type = 2;
				member_data->member_line = script->get_member_line(signal_name);
				member_data->member_method_info = signal_info;
				script_members_to_line[signal_name] = member_data;
			}
		}

		const Array members_to_keys = get_sorted_keys(script_members_to_line.keys());
		script_lines = p_code.split("\n");
		script_line_begin = get_script_line_begin(script_lines);
		/*script_json["script"] = */evaluate_script(p_script_json, members_to_keys);
	}
}

void GodoukenTranslatorV2::evaluate(nlohmann::json &p_script_json, const String& p_script_name, const String& p_script_directory) {
	//nlohmann::json script_translator_data; // = nlohmann::json::object();
	Error script_status;
	FileAccess *script_file = FileAccess::open(p_script_directory + p_script_name, FileAccess::READ, &script_status);
	if (script_status == Error::OK) {
		/*script_translator_data["data"] =*/ evaluate(p_script_json, script_file->get_as_utf8_string());
	}
}

GodoukenTranslatorV2::GodoukenTranslatorV2() :
	script(nullptr),
	script_line_begin(-1),
	script_line_finish(-1) {

	script_type_documentation.insert(Variant::Type::BOOL, "https://docs.godotengine.org/en/3.2/classes/class_bool.html");
	script_type_documentation.insert(Variant::Type::INT, "https://docs.godotengine.org/en/3.2/classes/class_int.html");
	script_type_documentation.insert(Variant::Type::REAL, "https://docs.godotengine.org/en/3.2/classes/class_float.html");
	script_type_documentation.insert(Variant::Type::STRING, "https://docs.godotengine.org/en/3.2/classes/class_string.html");
	script_type_documentation.insert(Variant::Type::VECTOR2, "https://docs.godotengine.org/en/3.2/classes/class_vector2.html");
	script_type_documentation.insert(Variant::Type::RECT2, "https://docs.godotengine.org/en/3.2/classes/class_rect2.html");
	script_type_documentation.insert(Variant::Type::VECTOR3, "https://docs.godotengine.org/en/3.2/classes/class_vector3.html");
	script_type_documentation.insert(Variant::Type::TRANSFORM2D, "https://docs.godotengine.org/en/3.2/classes/class_transform2d.html");
	script_type_documentation.insert(Variant::Type::PLANE, "https://docs.godotengine.org/en/3.2/classes/class_plane.html");
	script_type_documentation.insert(Variant::Type::QUAT, "https://docs.godotengine.org/en/3.2/classes/class_quat.html");
	script_type_documentation.insert(Variant::Type::AABB, "https://docs.godotengine.org/en/3.2/classes/class_aabb.html");
	script_type_documentation.insert(Variant::Type::BASIS, "https://docs.godotengine.org/en/3.2/classes/class_basis.html");
	script_type_documentation.insert(Variant::Type::TRANSFORM, "https://docs.godotengine.org/en/3.2/classes/class_transform.html");
	script_type_documentation.insert(Variant::Type::COLOR, "https://docs.godotengine.org/en/3.2/classes/class_color.html");
	script_type_documentation.insert(Variant::Type::NODE_PATH, "https://docs.godotengine.org/en/3.2/classes/class_nodepath.html");
	script_type_documentation.insert(Variant::Type::_RID, "https://docs.godotengine.org/en/3.2/classes/class_rid.html");
	script_type_documentation.insert(Variant::Type::DICTIONARY, "https://docs.godotengine.org/en/3.2/classes/class_dictionary.html");
	script_type_documentation.insert(Variant::Type::ARRAY, "https://docs.godotengine.org/en/3.2/classes/class_array.html");
	script_type_documentation.insert(Variant::Type::POOL_BYTE_ARRAY, "https://docs.godotengine.org/en/3.2/classes/class_poolbytearray.html");
	script_type_documentation.insert(Variant::Type::POOL_INT_ARRAY, "https://docs.godotengine.org/en/3.2/classes/class_poolintarray.html");
	script_type_documentation.insert(Variant::Type::POOL_REAL_ARRAY, "https://docs.godotengine.org/en/3.2/classes/class_poolrealarray.html");
	script_type_documentation.insert(Variant::Type::POOL_STRING_ARRAY, "https://docs.godotengine.org/en/3.2/classes/class_poolstringarray.html");
	script_type_documentation.insert(Variant::Type::POOL_VECTOR2_ARRAY, "https://docs.godotengine.org/en/3.2/classes/class_poolvector2array.html");
	script_type_documentation.insert(Variant::Type::POOL_VECTOR3_ARRAY, "https://docs.godotengine.org/en/3.2/classes/class_poolvector3array.html");
	script_type_documentation.insert(Variant::Type::POOL_COLOR_ARRAY, "https://docs.godotengine.org/en/3.2/classes/class_poolcolorarray.html");
	
	script_reserved_godot_methods.push_back("_init");
	script_reserved_godot_methods.push_back("_ready");
	script_reserved_godot_methods.push_back("_process");
	script_reserved_godot_methods.push_back("_process_physics");
	script_reserved_godot_methods.push_back("_enter_tree");
	script_reserved_godot_methods.push_back("_exit_tree");
}

GodoukenTranslatorV2::~GodoukenTranslatorV2() {

}
