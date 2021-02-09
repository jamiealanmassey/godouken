/* godouken_script_translator.cpp */

#include "godouken_script_translator.h"
#include "godouken_data_model.h"

#include "third_party/inja.hpp"
#include "third_party/crc.h"

/// GodoukenScriptTranslatorMemberData
GodoukenScriptTranslatorMemberData::GodoukenScriptTranslatorMemberData() {}
GodoukenScriptTranslatorMemberData::~GodoukenScriptTranslatorMemberData() {}

/// GodoukenScriptTranslatorCommentParser
void GodoukenScriptTranslatorCommentParser::parse(const List<String>& p_lines, const int32_t& p_from, const int32_t& p_to) {
	int index = 0;
	for (const List<String>::Element *E = p_lines.front(); E; E->next()) {
		if (index >= p_from) {
			const String &line = E->get();
			const int32_t find_at = line.find("@");
			const int32_t find_sep = line.find(":", find_at);
			if (find_at > 0 && find_sep > 0) {
				const String &keyword = line.substr(find_at, find_sep - find_at).strip_edges();
				const String &content = line.substr(find_sep).strip_edges();
				comment_entries[keyword] = content;
			}
		}

		index++;
		if (index >= p_to) {
			break;
		}
	}
}

void GodoukenScriptTranslatorCommentParser::parse(const List<String> &p_lines) {
	parse(p_lines, 0, p_lines.size());
}

void GodoukenScriptTranslatorCommentParser::flush() {
	comment_entries.clear();
}

const Dictionary &GodoukenScriptTranslatorCommentParser::get_comment_entires() const {
	return comment_entries;
}

const bool GodoukenScriptTranslatorCommentParser::has_parsed_entry(const String &p_key) const {
	return comment_entries.has(p_key);
}

const String &GodoukenScriptTranslatorCommentParser::get_parsed_entry(const String &p_key, const bool p_remove_element) {
	String result = "";
	if (comment_entries.has(p_key)) {
		result = comment_entries[p_key];
		if (p_remove_element) {
			comment_entries.erase(p_key);
		}
	}

	return result;
}

GodoukenScriptTranslatorCommentParser::GodoukenScriptTranslatorCommentParser() {}
GodoukenScriptTranslatorCommentParser::~GodoukenScriptTranslatorCommentParser() {}

/// GodoukenScriptTranslator
const Dictionary GodoukenScriptTranslator::get_members_to_line(const Ref<Script> &p_script) {
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

void GodoukenScriptTranslator::evaluate_member(const String &p_member_name) {
	if (script_members_to_line.has(p_member_name)) {
		translator_script_line_begin = translator_script->get_member_line(p_member_name);

		Object *script_member_obj = script_members_to_line[p_member_name];
		GodoukenScriptTranslatorMemberData *script_member_data = (GodoukenScriptTranslatorMemberData *)script_member_obj;
		if (script_member_data->member_type == 0) {

		} else if (script_member_data->member_type == 1) {
			MethodInfo method_info = translator_script->get_method_info(p_member_name);
			GodoukenDataNodeMethod method_data;
			GodoukenScriptTranslatorCommentParser *comment_data_parser = memnew(GodoukenScriptTranslatorCommentParser);
			comment_data_parser->parse(translator_script_lines, translator_script_line_begin, translator_script_line_end);
			method_data.method_is_godot = reserved_godot_methods.find(method_info.name);
			method_data.method_is_internal = method_info.name[0] == '_';
			method_data.node_name = method_info.name;
			method_data.node_desc_brief = comment_data_parser->get_parsed_entry("brief", true);
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
								method_parameter.param_type_data = GodoukenDataType(reserved_godot_types.find(property_type), property_type);
							}
						}

						method_data.method_params.push_back(method_parameter);
					}
				}
			}
		} else if (script_member_data->member_type == 2) {

		}
	}
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
			}

			for (index = 0; index < (*translator_script_methods).size(); index++) {
				const MethodInfo &method_info = (*translator_script_methods)[index];
				const String &method_name = method_info.name;

				GodoukenScriptTranslatorMemberData *member_data = memnew(GodoukenScriptTranslatorMemberData);
				member_data->member_type = 1;
				member_data->member_line = translator_script->get_member_line(method_name);
				script_members_to_line[method_name] = member_data;
			}

			for (index = 0; index < (*translator_script_signals).size(); index++) {
				const MethodInfo &signal_info = (*translator_script_signals)[index];
				const String &signal_name = signal_info.name;

				GodoukenScriptTranslatorMemberData *member_data = memnew(GodoukenScriptTranslatorMemberData);
				member_data->member_type = 2;
				member_data->member_line = translator_script->get_member_line(signal_name);
				script_members_to_line[signal_name] = member_data;
			}

			// const Dictionary &members_to_line = get_members_to_line(translator_script);
			const Array &members_to_keys = get_sorted_keys(script_members_to_line.keys());
			translator_script_lines = get_script_lines(translator_script_file);
			translator_script_line_begin = get_script_line_begin(translator_script_lines);
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
}






const Dictionary GodoukenTranslatorV2::get_members_to_line(const Ref<Script> &p_script) {
	Dictionary members_to_line = Dictionary();
	return members_to_line;
}

const Array &GodoukenTranslatorV2::get_sorted_keys(Array &p_keys) {
	if (p_keys.size() <= 1) {
		return p_keys;
	}

	Array keys_sorted = Array();
	Variant *keys_pure = new Variant[p_keys.size()];
	for (int32_t i = 0; i < p_keys.size(); i++) {
		int index = script->get_member_line(keys_pure[i]);
		int j = i;

		while (j > 0 && script->get_member_line(keys_pure[i - 1]) > index) {
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

const List<String> GodoukenTranslatorV2::get_script_lines(const FileAccess *p_script_file) {
	List<String> script_lines;
	String script_line = p_script_file->get_line();
	while (!(script_line = p_script_file->get_line()).empty()) {
		script_lines.push_back(script_line);
	}

	return script_lines;
}

const int32_t GodoukenTranslatorV2::get_script_line_begin(const List<String> &p_script_lines) {
	int32_t index = 0;
	for (const List<String>::Element *E = p_script_lines.front(); E; E = E->next()) {
		if (E->get().find("extends") >= 0) {
			return index;
		}
	}

	return -1;
}

nlohmann::json &GodoukenTranslatorV2::evaluate_property(const PropertyInfo &p_property_info) {
	GodoukenScriptTranslatorCommentParser *comment_parser = memnew(GodoukenScriptTranslatorCommentParser);
	comment_parser->parse(script_lines, script_line_begin, script_line_finish);

	nlohmann::json property_json = nlohmann::json::object();
	property_json["name"] = p_property_info.name.utf8();
	property_json["description"]["brief"] = "This is a short description";
	property_json["description"]["detailed"] = "This is a detailed description";
	property_json["type_info"]["name"] = "int";
	property_json["type_info"]["href"] = "https://docs.godotengine.org/en/3.2/classes/class_int.html";
	property_json["tags"]["is_godot"] = true;
	property_json["tags"]["is_exported"] = false;
	return property_json;
}

void GodoukenTranslatorV2::evaluate_member(const String& p_member_name) {
	if (script_members_to_line.has(p_member_name)) {
		script_line_begin = script->get_member_line(p_member_name);

		Object *script_member_obj = script_members_to_line[p_member_name];
		GodoukenScriptTranslatorMemberData *script_member_data = (GodoukenScriptTranslatorMemberData *)script_member_obj;
		if (script_member_data->member_type == 0) {

		} else if (script_member_data->member_type == 1) {
			MethodInfo method_info = script->get_method_info(p_member_name);
			GodoukenDataNodeMethod method_data;
			GodoukenScriptTranslatorCommentParser *comment_data_parser = memnew(GodoukenScriptTranslatorCommentParser);
			comment_data_parser->parse(script_lines, script_line_begin, script_line_finish);
			method_data.method_is_godot = script_reserved_godot_methods.find(method_info.name);
			method_data.method_is_internal = method_info.name[0] == '_';
			method_data.node_name = method_info.name;
			method_data.node_desc_brief = comment_data_parser->get_parsed_entry("brief", true);
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
			}
		} else if (script_member_data->member_type == 2) {
		}

		script_line_finish = script_line_begin;
	}
}

nlohmann::json& GodoukenTranslatorV2::evaluate(const String& p_script_name, const String& p_script_directory) {
	nlohmann::json translator_data = nlohmann::json::object();
	Error *script_status = nullptr;
	FileAccess *script_file = nullptr;
	//GodoukenDataModel *translator_data_model = nullptr;

	script_file = FileAccess::open(p_script_directory + p_script_name, FileAccess::READ, script_status);
	if (script_status && (*script_status) == Error::OK) {
		script->set_source_code(script_file->get_as_utf8_string());

		if (script->is_valid()) {
			//data_model = memnew(GodoukenDataModel);
			List<MethodInfo> *script_methods = nullptr;
			List<MethodInfo> *script_signals = nullptr;
			List<PropertyInfo> *script_properties = nullptr;

			script->get_script_method_list(script_methods);
			script->get_script_signal_list(script_signals);
			script->get_script_property_list(script_properties);

			int32_t index = 0;
			for (index = 0; index < (*script_properties).size(); index++) {
				const PropertyInfo &property_info = (*script_properties)[index];
				const String &property_name = property_info.name;

				GodoukenScriptTranslatorMemberData *member_data = memnew(GodoukenScriptTranslatorMemberData);
				member_data->member_type = 0;
				member_data->member_line = script->get_member_line(property_name);
				script_members_to_line[property_name] = member_data;
			}

			for (index = 0; index < (*script_methods).size(); index++) {
				const MethodInfo &method_info = (*script_methods)[index];
				const String &method_name = method_info.name;

				GodoukenScriptTranslatorMemberData *member_data = memnew(GodoukenScriptTranslatorMemberData);
				member_data->member_type = 1;
				member_data->member_line = script->get_member_line(method_name);
				script_members_to_line[method_name] = member_data;
			}

			for (index = 0; index < (*script_signals).size(); index++) {
				const MethodInfo &signal_info = (*script_signals)[index];
				const String &signal_name = signal_info.name;

				GodoukenScriptTranslatorMemberData *member_data = memnew(GodoukenScriptTranslatorMemberData);
				member_data->member_type = 2;
				member_data->member_line = script->get_member_line(signal_name);
				script_members_to_line[signal_name] = member_data;
			}

			// const Dictionary &members_to_line = get_members_to_line(translator_script);
			const Array &members_to_keys = get_sorted_keys(script_members_to_line.keys());
			script_lines = get_script_lines(script_file);
			script_line_begin = get_script_line_begin(script_lines);
		}
	}

	return translator_data;
}

GodoukenTranslatorV2::GodoukenTranslatorV2() {
	script = nullptr;
	script_line_begin = -1;
	script_line_finish = -1;

	script_reserved_godot_methods.push_back("_init");
	script_reserved_godot_methods.push_back("_ready");
	script_reserved_godot_methods.push_back("_process");
	script_reserved_godot_methods.push_back("_process_physics");
	script_reserved_godot_methods.push_back("_enter_tree");
	script_reserved_godot_methods.push_back("_exit_tree");
	script_reserved_godot_types.push_back("bool");
	script_reserved_godot_types.push_back("int");
	script_reserved_godot_types.push_back("float");
	script_reserved_godot_types.push_back("String");
	script_reserved_godot_types.push_back("Vector2");
	script_reserved_godot_types.push_back("Vector3");
	script_reserved_godot_types.push_back("Rect2");
	script_reserved_godot_types.push_back("Transform2D");
	script_reserved_godot_types.push_back("Transform");
	script_reserved_godot_types.push_back("Plane");
	script_reserved_godot_types.push_back("Quat");
	script_reserved_godot_types.push_back("AABB");
	script_reserved_godot_types.push_back("Basis");
	script_reserved_godot_types.push_back("Color");
	script_reserved_godot_types.push_back("NodePath");
	script_reserved_godot_types.push_back("RID");
	script_reserved_godot_types.push_back("Object");
	script_reserved_godot_types.push_back("Array");
	script_reserved_godot_types.push_back("Dictionary");
	script_reserved_godot_types.push_back("PoolByteArray");
	script_reserved_godot_types.push_back("PoolIntArray");
	script_reserved_godot_types.push_back("PoolRealArray");
	script_reserved_godot_types.push_back("PoolStringArray");
	script_reserved_godot_types.push_back("PoolVector2Array");
	script_reserved_godot_types.push_back("PoolVector3Array");
	script_reserved_godot_types.push_back("PoolColorArray");
}

GodoukenTranslatorV2::~GodoukenTranslatorV2() {

}
