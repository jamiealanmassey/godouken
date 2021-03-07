/* godouken_script_translator.h */

#ifndef GODOUKEN_SCRIPT_TRANSLATOR_H
#define GODOUKEN_SCRIPT_TRANSLATOR_H

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

class GodoukenScriptTranslatorMemberData : public Object {
	GDCLASS(GodoukenScriptTranslatorMemberData, Object);

public:
	uint8_t member_type;
	uint32_t member_line;

public:
	GodoukenScriptTranslatorMemberData();
	~GodoukenScriptTranslatorMemberData();
};

class GodoukenScriptTranslatorPropertyData : public GodoukenScriptTranslatorMemberData {
	GDCLASS(GodoukenScriptTranslatorPropertyData, GodoukenScriptTranslatorMemberData);

public:
	PropertyInfo member_property_info;

public:
	GodoukenScriptTranslatorPropertyData();
	~GodoukenScriptTranslatorPropertyData();
};

class GodoukenScriptTranslatorMethodData : public GodoukenScriptTranslatorMemberData {
	GDCLASS(GodoukenScriptTranslatorMethodData, GodoukenScriptTranslatorMemberData);

public:
	MethodInfo member_method_info;

public:
	GodoukenScriptTranslatorMethodData();
	~GodoukenScriptTranslatorMethodData();
};

class GodoukenScriptTranslatorCommentData : public Object {
    GDCLASS(GodoukenScriptTranslatorCommentData, Object);

public:
    String comment_body;
    String comment_selector;

public:
    GodoukenScriptTranslatorCommentData();
    GodoukenScriptTranslatorCommentData(const GodoukenScriptTranslatorCommentData &p_comment_data);
    GodoukenScriptTranslatorCommentData(const String &p_comment_body, const String &p_comment_selector);
};

typedef Vector<GodoukenScriptTranslatorCommentData *> CommentDataSet;
typedef Map<String, CommentDataSet> CommentDataStore;

static const CommentDataSet &CommentDataSet_0 = CommentDataSet();
static const CommentDataStore &CommentDataStore_0 = CommentDataStore();

class GodoukenScriptTranslatorCommentParser : public Object {
    GDCLASS(GodoukenScriptTranslatorCommentParser, Object);

protected:
    CommentDataStore comment_entries;

public:
	void parse(const Vector<String> &p_lines, const int32_t &p_from, const int32_t &p_to);
	void parse(const Vector<String> &p_lines);
	void flush();

	const CommentDataStore &get_data_store() const;
    const CommentDataSet &get_data_set(const String &p_key) const;
	const bool has_data_set(const String &p_key) const;
	const GodoukenScriptTranslatorCommentData *get_data_set_first(const String &p_key/*, const bool p_remove_element = false*/);
    
public:
	GodoukenScriptTranslatorCommentParser();
	~GodoukenScriptTranslatorCommentParser();
};

class GodoukenScriptTranslator : public Object {
    GDCLASS(GodoukenScriptTranslator, Object);

protected:
    GDScript *translator_script;
    int32_t translator_script_line_begin;
    int32_t translator_script_line_end;

	List<String> translator_script_lines;
	List<String> translator_script_keys;

    List<String> reserved_godot_methods;
    List<String> reserved_godot_types;

    Map<String, GodoukenDataNodeProperty> script_data_properties;
    Map<String, GodoukenDataNodeMethod> script_data_methods;
    Map<String, GodoukenDataNodeSignal> script_data_signals;

	Map<String, PropertyInfo> script_property_infos;
	Map<String, MethodInfo> script_method_infos;
	Map<String, MethodInfo> script_signal_infos;

	Dictionary script_members_to_line;

private:
    const Dictionary get_members_to_line(const Ref<Script> &p_script);
    const Array &get_sorted_keys(Array &p_keys);

    const List<String> get_script_lines(const FileAccess *p_script_file);
    const int32_t get_script_line_begin(const List<String> &p_script_lines);

protected:
    const GodoukenDataNodeMeta get_meta_data() const;

	void evaluate(const String &p_member_name);

	void evaluate_property(const String &p_member_name, GodoukenScriptTranslatorCommentParser *p_comment_parser);
	void evaluate_method(const String &p_member_name, GodoukenScriptTranslatorCommentParser *p_comment_parser);
	void evaluate_signal(const String &p_member_name, GodoukenScriptTranslatorCommentParser *p_comment_parser);

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

	Vector<String> script_lines;
	List<String> script_keys;
	List<String> script_reserved_godot_methods;
	List<String> script_reserved_godot_types;

	Dictionary script_members_to_line;

private:
	const Dictionary get_members_to_line(const Ref<Script> &p_script);
	const bool get_sorted_key(const Variant &p_variant_1, const Variant &p_variant_2);
	const Array &get_sorted_keys(Array &p_keys);

	const List<String> get_script_lines(const FileAccess *p_script_file);
	const int32_t get_script_line_begin(const Vector<String> &p_script_lines);

protected:
	void evaluate_member(const String &p_member_name);

public:
	void evaluate_signal(nlohmann::json &p_script_json, const MethodInfo &p_signal_info);
	void evaluate_method(nlohmann::json &p_script_json, const MethodInfo &p_method_info);
	void evaluate_property(nlohmann::json &p_script_json, const PropertyInfo &p_property_info);
	void evaluate_script(nlohmann::json &p_script_json, const Array p_members_to_keys);
	void evaluate(nlohmann::json &p_script_json, const String &p_code);
	void evaluate(nlohmann::json &p_script_json, const String &p_script_name, const String &p_script_directory);

public:
	GodoukenTranslatorV2();
	~GodoukenTranslatorV2();
};


#endif // GODOUKEN_SCRIPT_TRANSLATOR_H
