/* godouken_script_translator.cpp */

#include "godouken_script_translator.h"
#include "godouken_data_model.h"
#include "modules/regex/regex.h"

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
					comment_selector = line.substr(brace_0 + 1, brace_1 - brace_0 - 1).strip_edges();
					comment_keyword = comment_keyword.substr(0, comment_keyword.find("["));
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

const Dictionary GodoukenTranslator::get_members_to_line(const Ref<Script> &p_script) {
	Dictionary members_to_line = Dictionary();
	return members_to_line;
}

const Array &GodoukenTranslator::get_sorted_keys(Array &p_keys) const {
	if (p_keys.size() <= 1) {
		return p_keys;
	}

	int32_t i = 0;
	int32_t j = 0;
	int32_t min_idx = 0;
	for (i = 0; i < p_keys.size() - 1; i++)
	{ 
		min_idx = i;
		for (j = i + 1; j < p_keys.size(); j++) {
			const int32_t key_1 = script->get_member_line(p_keys[j]);
			const int32_t key_2 = script->get_member_line(p_keys[min_idx]);
			if (key_1 < key_2) {
				min_idx = j;
			}
		}

		const Variant temp = p_keys[min_idx];
		p_keys[min_idx] = p_keys[i];
		p_keys[i] = temp;
	}
	
	return p_keys;
}

List<String> GodoukenTranslator::get_script_lines(const FileAccess *p_script_file) {
	List<String> script_lines;
	String script_line = p_script_file->get_line();
	while (!(script_line = p_script_file->get_line()).empty()) {
		script_lines.push_back(script_line);
	}

	return script_lines;
}

int32_t GodoukenTranslator::get_script_line_begin(const Vector<String> &p_script_lines) {
	int32_t index = -1;
	for (int32_t i = 0; i < p_script_lines.size(); i++) {
		if (p_script_lines[i].find("extends") >= 0) {
			index = i;
			break;
		}
	}

	return index;
}

void GodoukenTranslator::get_type_info(String &p_type_name, String &p_type_href, bool &p_type_is_godot, const PropertyInfo &p_property_info) {
	StringBuilder type_builder = StringBuilder();
	p_type_name = Variant::get_type_name(p_property_info.type).to_lower();
	p_type_is_godot = p_property_info.type != Variant::NIL && p_property_info.type != Variant::OBJECT;
	if (p_type_is_godot) {
		type_builder += "https://docs.godotengine.org/en/stable/classes/class_";
		type_builder += p_type_name;
		type_builder += ".html";
	}

	p_type_href = type_builder.as_string();
}

void GodoukenTranslator::populate(nlohmann::json &p_script_json) {
	p_script_json["data"]["script"]["name"] = "";
	p_script_json["data"]["script"]["name_sm"] = "";
	p_script_json["data"]["script"]["name_dt"] = "";
	p_script_json["data"]["script"]["diagram"] = "";
	p_script_json["data"]["script"]["base"]["name"] = "";
	p_script_json["data"]["script"]["base"]["color"] = "#FFFFFF";
	p_script_json["data"]["script"]["description"]["brief"] = "";
	p_script_json["data"]["script"]["description"]["detailed"] = "";
	p_script_json["data"]["script"]["meta"]["deprecated"] = "";
	p_script_json["data"]["script"]["meta"]["version"] = "";
	p_script_json["data"]["script"]["meta"]["created"] = "";
	p_script_json["data"]["script"]["meta"]["modified"] = "";
	p_script_json["data"]["script"]["meta"]["collection"] = "";
	p_script_json["data"]["script"]["meta"]["author"] = "";
	p_script_json["data"]["script"]["properties"] = nlohmann::json::array();
	p_script_json["data"]["script"]["methods"] = nlohmann::json::array();
	p_script_json["data"]["script"]["signals"] = nlohmann::json::array();
}

void GodoukenTranslator::evaluate_signal(nlohmann::json &p_script_json, const MethodInfo &p_signal_info) const {
	GodoukenScriptTranslatorCommentParser *comment_parser = memnew(GodoukenScriptTranslatorCommentParser);
	comment_parser->parse(script_lines, script_line_begin, script_line_finish);

	nlohmann::json signal_json = nlohmann::json::object();
	signal_json["name"] = p_signal_info.name.utf8();
	p_script_json["data"]["script"]["signals"].push_back(signal_json);
}

bool contains(const List<String> &p_data, const String &p_keyword) {
	bool is_found = false;
	for (int32_t i = 0; i < p_data.size(); i++) {
		if (p_data[i].find(p_keyword) > 0) {
			is_found = true;
		}
	}

	return is_found;
}

void GodoukenTranslator::evaluate_method(nlohmann::json &p_script_json, const MethodInfo &p_method_info) const {
	GodoukenScriptTranslatorCommentParser *comment_parser = memnew(GodoukenScriptTranslatorCommentParser);
	comment_parser->parse(script_lines, script_line_begin, script_line_finish);

	bool is_virtual = (p_method_info.flags & METHOD_FLAG_VIRTUAL) == METHOD_FLAG_VIRTUAL;
	bool type_is_godot = false;
	String type_name = "";
	String type_href = "";
	get_type_info(type_name, type_href, type_is_godot, p_method_info.return_val);
	
	nlohmann::json method_json = nlohmann::json::object();
	method_json["name"] = p_method_info.name.utf8();
	method_json["description"]["brief"] = comment_parser->get_data_set_first("@brief")->comment_body.utf8();
	method_json["description"]["detailed"] = comment_parser->get_data_set_first("@detailed")->comment_body.utf8();
	method_json["tags"]["is_godot"] = contains(script_reserved_godot_methods, p_method_info.name);
	method_json["tags"]["is_private"] = p_method_info.name.size() > 0 && p_method_info.name[0] == '_';
	method_json["tags"]["is_internal"] = false;
	method_json["tags"]["is_overrider"] = is_virtual;
	method_json["tags"]["cl_overrider"] = false;
	method_json["extra"]["warning"] = comment_parser->get_data_set_first("@warning")->comment_body.utf8();
	method_json["extra"]["info"] = comment_parser->get_data_set_first("@info")->comment_body.utf8();
	method_json["parameters"] = nlohmann::json::array();
	method_json["see"] = nlohmann::json::array();
	
	method_json["return"]["description"] = comment_parser->get_data_set_first("@return")->comment_body.utf8();
	method_json["return"]["type_info"]["name"] = type_name.utf8();
	method_json["return"]["type_info"]["href"] = type_href.utf8();
	
	const CommentDataSet &comment_args_set = comment_parser->get_data_set("@parameter");
	for (int32_t x = 0; x < p_method_info.arguments.size(); x++) {
		const PropertyInfo &property_info = p_method_info.arguments[x];
		String type_arg_name = "";
		String type_arg_href = "";
		bool type_arg_is_godot = false;
		get_type_info(type_arg_name, type_arg_href, type_arg_is_godot, property_info);
		
		nlohmann::json method_parameter_json = nlohmann::json::object();
		method_parameter_json["name"] = property_info.name.utf8();
		method_parameter_json["description"] = "";
		method_parameter_json["type_info"]["name"] = type_arg_name.utf8();
		method_parameter_json["type_info"]["href"] = type_arg_href.utf8();

		if (x < comment_args_set.size()) {
			const String &comment_selector = comment_args_set[x]->comment_selector;
			method_parameter_json["description"] = comment_args_set[x]->comment_body.utf8();
			method_parameter_json["name"] = !comment_selector.empty() ? comment_selector.utf8() : std::to_string(x).c_str();
		}
		
		/*for (int32_t i = 0; i < comment_args_set.size(); i++) {
			if (p_method_info.arguments[x].name.find(comment_args_set[i]->comment_selector) == 0) {
				method_parameter_json["description"] = comment_args_set[i]->comment_body.utf8();
				break;
			}
		}*/

		method_json["parameters"].push_back(method_parameter_json);
	}

	const CommentDataSet &comment_also_set = comment_parser->get_data_set("@see");
	for (int32_t i = 0; i < comment_also_set.size(); i++) {
		method_json["see"].push_back(comment_also_set[i]->comment_body.utf8());
	}

	p_script_json["data"]["script"]["methods"].push_back(method_json);
}

void GodoukenTranslator::evaluate_property(nlohmann::json &p_script_json, const PropertyInfo &p_property_info) const {
	GodoukenScriptTranslatorCommentParser *comment_parser = memnew(GodoukenScriptTranslatorCommentParser);
	comment_parser->parse(script_lines, script_line_begin, script_line_finish);

	bool type_is_exported = (p_property_info.usage & PROPERTY_USAGE_EDITOR) == PROPERTY_USAGE_EDITOR;
	bool type_is_godot = false;
	String type_name = "";
	String type_href = "";
	get_type_info(type_name, type_href, type_is_godot, p_property_info);
	
	nlohmann::json property_json = nlohmann::json::object();
	property_json["name"] = p_property_info.name.utf8();
	property_json["description"]["brief"] = comment_parser->get_data_set_first("@brief")->comment_body.utf8();
	property_json["description"]["detailed"] = comment_parser->get_data_set_first("@detailed")->comment_body.utf8();
	property_json["type_info"]["name"] = type_name.utf8();
	property_json["type_info"]["href"] = type_href.utf8();
	property_json["tags"]["is_godot"] = type_is_godot;
	property_json["tags"]["is_exported"] = type_is_exported;
	p_script_json["data"]["script"]["properties"].push_back(property_json);
}

void GodoukenTranslator::evaluate_script(nlohmann::json &p_script_json, const Array &p_members_to_keys) {
	GodoukenScriptTranslatorCommentParser *comment_parser = memnew(GodoukenScriptTranslatorCommentParser);
	comment_parser->parse(script_lines, 0, script_line_begin);
	const String &script_name_parse = comment_parser->get_data_set_first("@name")->comment_body;
	const String &script_name_prev = p_script_json["data"]["script"]["name"].get<std::string>().c_str();
	const String &script_name = script_name_parse.empty() ? script_name_prev : script_name_parse;

	p_script_json["data"]["script"]["name"] = script_name.utf8();
	p_script_json["data"]["script"]["name_sm"] = script_name.replace(" ", "_").to_lower().utf8();
	p_script_json["data"]["script"]["name_dt"] = script_name.replace(" ", "_").replace("_", " ").capitalize().replace(" ", "").utf8();
	p_script_json["data"]["script"]["description"]["brief"] = comment_parser->get_data_set_first("@brief")->comment_body.utf8();
	p_script_json["data"]["script"]["description"]["detailed"] = comment_parser->get_data_set_first("@detailed")->comment_body.utf8();
	p_script_json["data"]["script"]["meta"]["deprecated"] = comment_parser->get_data_set_first("@deprecated")->comment_body.utf8();
	p_script_json["data"]["script"]["meta"]["version"] = comment_parser->get_data_set_first("@version")->comment_body.utf8();
	p_script_json["data"]["script"]["meta"]["created"] = comment_parser->get_data_set_first("@created")->comment_body.utf8();
	p_script_json["data"]["script"]["meta"]["modified"] = comment_parser->get_data_set_first("@modified")->comment_body.utf8();
	p_script_json["data"]["script"]["meta"]["collection"] = comment_parser->get_data_set_first("@collection")->comment_body.utf8();
	p_script_json["data"]["script"]["meta"]["author"] = comment_parser->get_data_set_first("@author")->comment_body.utf8();

	for (int32_t i = 0; i < p_members_to_keys.size(); i++) {
		const String &member_name = p_members_to_keys.get(i);
		Object *member_data_ptr = script_members_to_line[member_name];
		const GodoukenScriptTranslatorMemberData *member_data = dynamic_cast<GodoukenScriptTranslatorMemberData *>(member_data_ptr);
		if (member_data) {
			script_line_finish = member_data->member_line;
			switch (member_data->member_type) {
				case 0:
					evaluate_property(p_script_json, (dynamic_cast<const GodoukenScriptTranslatorPropertyData *>(member_data))->member_property_info);
					break;
				case 1:
					evaluate_method(p_script_json, (dynamic_cast<const GodoukenScriptTranslatorMethodData *>(member_data))->member_method_info);
					break;
				case 2:
					evaluate_signal(p_script_json, (dynamic_cast<const GodoukenScriptTranslatorMethodData *>(member_data))->member_method_info);
					break;
				default:
					break;
			}
		}

		script_line_begin = script_line_finish;
	}
}

void GodoukenTranslator::evaluate(nlohmann::json &p_script_json, const String &p_code) {
	GodoukenTranslator::populate(p_script_json);
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
		p_script_json["data"]["script"]["name"] = script->get_name().utf8();
		
		const Map<StringName, Ref<GDScript>> &subclasses = script->get_subclasses();
		const Ref<Script> base_script = script->get_base_script();
		const Ref<GDScriptNativeClass> &base_native = script->get_native();
		if (base_script.is_valid()) {
			const String &base_path = base_script->get_path();
			const int base_split = base_path.find_last("/");
			const int base_end = base_path.find_char('.', base_split);
			const String &base_name = base_path.substr(base_split + 1, base_end - base_split - 1);
			const String &base_name_perm = base_name.replace("_", " ").capitalize().replace(" ", "");
			p_script_json["data"]["script"]["base"]["name"] = base_name_perm.utf8();
			p_script_json["data"]["script"]["base"]["color"] = "#FFFFFF";
		}
		else if (base_native.is_valid()) {
			const String &base_name = base_native->get_name().operator String();
			p_script_json["data"]["script"]["base"]["name"] = base_name.utf8();
			p_script_json["data"]["script"]["base"]["color"] = "#44FFBB";
		}
		
		uint32_t index = 0;
		uint32_t index_max = 4294967295;
		if (script_properties) {
			for (index = 0; index < script_properties->size(); index++) {
				GodoukenScriptTranslatorPropertyData *member_data = memnew(GodoukenScriptTranslatorPropertyData);
				const PropertyInfo &property_info = (*script_properties)[index];
				const String &property_name = property_info.name;

				member_data->member_type = 0;
				member_data->member_line = script->get_member_line(property_name);
				member_data->member_property_info = property_info;
				if (member_data->member_line < index_max) {
					script_members_to_line[property_name] = member_data;
				}
			}
		}

		if (script_methods) {
			for (index = 0; index < script_methods->size(); index++) {
				GodoukenScriptTranslatorMethodData *member_data = memnew(GodoukenScriptTranslatorMethodData);
				const MethodInfo &method_info = (*script_methods)[index];
				const String &method_name = method_info.name;

				member_data->member_type = 1;
				member_data->member_line = script->get_member_line(method_name);
				member_data->member_method_info = method_info;
				if (member_data->member_line < index_max) {
					script_members_to_line[method_name] = member_data;
				}
			}
		}

		if (script_signals) {
			for (index = 0; index < script_signals->size(); index++) {
				GodoukenScriptTranslatorMethodData *member_data = memnew(GodoukenScriptTranslatorMethodData);
				const MethodInfo &signal_info = (*script_signals)[index];
				const String &signal_name = signal_info.name;

				member_data->member_type = 2;
				member_data->member_line = script->get_member_line(signal_name);
				member_data->member_method_info = signal_info;
				if (member_data->member_line < index_max) {
					script_members_to_line[signal_name] = member_data;
				}
			}
		}

		Set<StringName> *members = new Set<StringName>();
		script->get_members(members);
		
		const Array members_to_keys = get_sorted_keys(script_members_to_line.keys());
		script_lines = p_code.split("\n");
		script_line_begin = get_script_line_begin(script_lines);
		evaluate_script(p_script_json, members_to_keys);
	}
}

void GodoukenTranslator::evaluate(nlohmann::json &p_script_json, const String& p_script_name, const String& p_script_directory) {
	Error script_status;
	FileAccess *script_file = FileAccess::open(p_script_directory + p_script_name, FileAccess::READ, &script_status);
	if (script_status == Error::OK) {
		evaluate(p_script_json, script_file->get_as_utf8_string());
	}
}

GodoukenTranslator::GodoukenTranslator() :
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

GodoukenTranslator::~GodoukenTranslator() {

}
