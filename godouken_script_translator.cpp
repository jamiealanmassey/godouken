/* godouken_script_translator.cpp */

#include "godouken_script_translator.h"
#include "godouken_data_model.h"

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
