/* godouken_script_translator.cpp */

#include "godouken_script_translator.h"
#include "godouken_data_model.h"

#include <typeinfo>

/// GodoukenScriptTranslatorMemberData
GodoukenScriptTranslatorMemberData::GodoukenScriptTranslatorMemberData() {}
GodoukenScriptTranslatorMemberData::~GodoukenScriptTranslatorMemberData() {}

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

void GodoukenScriptTranslatorCommentParser::parse(const List<String>& p_lines, const int32_t& p_from, const int32_t& p_to) {
	int index = 0;
	for (const List<String>::Element *E = p_lines.front(); E; E->next()) {
		if (index >= p_from) {
			String comment_keyword = "";
			String comment_content = "";
			String comment_selector = "";

			const String &line = E->get();
			const int32_t find_at = line.find("@");
			const int32_t find_sep = line.find(":", find_at);
			if (find_at > 0 && find_sep > 0) {
				comment_keyword = line.substr(find_at, find_sep - find_at).strip_edges();
				comment_content = line.substr(find_sep).strip_edges();

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

void GodoukenScriptTranslatorCommentParser::parse(const List<String> &p_lines) {
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
	GodoukenScriptTranslatorCommentData *result = nullptr;
	if (comment_entries.has(p_key)) {
		const CommentDataSet &comment_data_set = comment_entries[p_key];
		if (comment_data_set.size() > 0) {
			result = comment_data_set[0];
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
