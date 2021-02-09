/* godouken_script_translator.h */

#include "godouken_data_types.h"

#include "core/script_language.h"
#include "core/object.h"
#include "core/map.h"
#include "core/vector.h"
#include "core/array.h"
#include "core/list.h"
#include "core/ustring.h"
#include "core/dictionary.h"
#include "core/ustring.h"
#include "core/string_builder.h"
#include "core/os/file_access.h"

#include "modules/gdscript/gdscript.h"

#ifndef GODOUKEN_SCRIPT_TRANSLATOR_H
#define GODOUKEN_SCRIPT_TRANSLATOR_H

class GodoukenScriptTranslatorMemberData : public Object {
	GDCLASS(GodoukenScriptTranslatorMemberData, Object);

public:
	uint8_t member_type;
	uint32_t member_line; 

public:
	GodoukenScriptTranslatorMemberData();
	~GodoukenScriptTranslatorMemberData();
};

class GodoukenScriptTranslatorCommentParser : public Object {
    GDCLASS(GodoukenScriptTranslatorCommentParser, Object);

protected:
	Dictionary comment_entries;

public:
	void parse(const List<String> &p_lines, const int32_t &p_from, const int32_t &p_to);
	void parse(const List<String> &p_lines);
	void flush();

	const Dictionary &get_comment_entires() const;
	const bool has_parsed_entry(const String &p_key) const;
	const String &get_parsed_entry(const String &p_key, const bool p_remove_element = false);


public:
	GodoukenScriptTranslatorCommentParser();
	~GodoukenScriptTranslatorCommentParser();
};

class GodoukenScriptTranslator : public Object {
    GDCLASS(GodoukenScriptTranslator, Object);

protected:
    GDScript* translator_script;
    int32_t translator_script_line_begin;
    int32_t translator_script_line_end;

	List<String> translator_script_lines;
	List<String> translator_script_keys;

    List<String> reserved_godot_methods;
    List<String> reserved_godot_types;

    Map<String, GodoukenDataNodeProperty> script_data_properties;
    Map<String, GodoukenDataNodeMethod> script_data_methods;
    Map<String, GodoukenDataNodeSignal> script_data_signals;

	Dictionary script_members_to_line;

private:
    const Dictionary get_members_to_line(const Ref<Script> &p_script);
    const Array &get_sorted_keys(Array &p_keys);

    const List<String> get_script_lines(const FileAccess *p_script_file);
    const int32_t get_script_line_begin(const List<String> &p_script_lines);

protected:
    const GodoukenDataNodeMeta get_meta_data() const;

	void evaluate_member(const String &p_member_name);

public:
    class GodoukenDataModel *script_translate(const String &p_script_name, const String &p_script_directory);

public:
    GodoukenScriptTranslator();
    ~GodoukenScriptTranslator();
};


#include "third_party/json.hpp"

class GodoukenTranslatorV2 : public Object {
	GDCLASS(GodoukenTranslatorV2, Object);

protected:
	GDScript *script;
	int32_t script_line_begin;
	int32_t script_line_finish;

	List<String> script_lines;
	List<String> script_keys;
	List<String> script_reserved_godot_methods;
	List<String> script_reserved_godot_types;

	Dictionary script_members_to_line;

private:
	const Dictionary get_members_to_line(const Ref<Script> &p_script);
	const Array &get_sorted_keys(Array &p_keys);

	const List<String> get_script_lines(const FileAccess *p_script_file);
	const int32_t get_script_line_begin(const List<String> &p_script_lines);

protected:
	void evaluate_member(const String &p_member_name);

public:
	nlohmann::json &evaluate_property(const PropertyInfo &p_property_info);
	nlohmann::json &evaluate(const String &p_script_name, const String &p_script_directory);

public:
	GodoukenTranslatorV2();
	~GodoukenTranslatorV2();
};


#endif // GODOUKEN_SCRIPT_TRANSLATOR_H
