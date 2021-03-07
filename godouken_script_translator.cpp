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

const Dictionary GodoukenTranslatorV2::get_members_to_line(const Ref<Script> &p_script) {
	Dictionary members_to_line = Dictionary();
	return members_to_line;
}

const bool GodoukenTranslatorV2::get_sorted_key(const Variant & p_variant_1, const Variant & p_variant_2)
{
	const Object *method_data_obj_1 = p_variant_1.operator Object*();
	const Object *method_data_obj_2 = p_variant_2.operator Object*();
	const GodoukenScriptTranslatorMethodData *method_data_1 = static_cast<const GodoukenScriptTranslatorMethodData *>(method_data_obj_1);
	const GodoukenScriptTranslatorMethodData *method_data_2 = static_cast<const GodoukenScriptTranslatorMethodData *>(method_data_obj_2);
	return method_data_1->member_line < method_data_2->member_line;
}

const Array &GodoukenTranslatorV2::get_sorted_keys(Array &p_keys) {
	if (p_keys.size() <= 1) {
		return p_keys;
	}

	Array keys_sorted = Array(p_keys);
	keys_sorted.sort_custom(this, "get_sorted_key");
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

	GodoukenScriptTranslatorCommentParser *comment_parser = memnew(GodoukenScriptTranslatorCommentParser);
	comment_parser->parse(script_lines, script_line_begin, script_line_finish);

	nlohmann::json method_json = nlohmann::json::object();
}

void GodoukenTranslatorV2::evaluate_property(nlohmann::json &p_script_json, const PropertyInfo &p_property_info) {
	GodoukenScriptTranslatorCommentParser *comment_parser = memnew(GodoukenScriptTranslatorCommentParser);
	comment_parser->parse(script_lines, script_line_begin, script_line_finish);

	bool type_is_godot = p_property_info.type != Variant::NIL && p_property_info.type != Variant::OBJECT;
	bool type_is_exported = (p_property_info.usage & PROPERTY_USAGE_EDITOR) == PROPERTY_USAGE_EDITOR;
	
	String type_str = Variant::get_type_name(p_property_info.type).to_lower();
	StringBuilder type_builder = StringBuilder();
	if (type_is_godot) {
		type_builder += "https://docs.godotengine.org/en/stable/classes/class_";
		type_builder += type_str;
		type_builder += ".html";
	}
	
	nlohmann::json property_json = nlohmann::json::object();
	property_json["name"] = p_property_info.name.utf8();
	property_json["description"]["brief"] = comment_parser->get_data_set_first("@brief")->comment_body.utf8();
	property_json["description"]["detailed"] = comment_parser->get_data_set_first("@detailed")->comment_body.utf8();
	property_json["type_info"]["name"] = type_str.utf8();
	property_json["type_info"]["href"] = type_builder.as_string().utf8();
	property_json["tags"]["is_godot"] = type_is_godot;
	property_json["tags"]["is_exported"] = type_is_exported;
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
	
	script_reserved_godot_methods.push_back("_init");
	script_reserved_godot_methods.push_back("_ready");
	script_reserved_godot_methods.push_back("_process");
	script_reserved_godot_methods.push_back("_process_physics");
	script_reserved_godot_methods.push_back("_enter_tree");
	script_reserved_godot_methods.push_back("_exit_tree");
}

GodoukenTranslatorV2::~GodoukenTranslatorV2() {

}
