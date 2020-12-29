/* godouken_data_types.h */

#include "core/ustring.h"

#ifndef GODOUKEN_DATA_TYPES_H
#define GODOUKEN_DATA_TYPES_H

struct GodoukenDataMetaRoadmap
{
	GodoukenDataMetaRoadmap(
			const String &p_roadmap_version,
			const String &p_roadmap_desc) :

			roadmap_version(p_roadmap_version),
			roadmap_desc(p_roadmap_desc)

	{}

    String roadmap_version;
    String roadmap_desc;
};

struct GodoukenDataMetaLink 
{
	GodoukenDataMetaLink(
			const String &p_link_label,
			const String &p_link_address) :

			link_label(p_link_label),
			link_address(p_link_address)

	{}

    String link_label;
    String link_address;
};

struct GodoukenDataMetaTodo
{
	GodoukenDataMetaTodo(
			const String &p_todo_owner,
			const String &p_todo_desc,
			const int p_todo_line_num) :

			todo_owner(p_todo_owner),
			todo_desc(p_todo_desc),
			todo_line_num(p_todo_line_num)

	{}

    String todo_owner;
    String todo_desc;
    int todo_line_num;
};

struct GodoukenDataNodeMeta
{
	GodoukenDataNodeMeta(
			const float p_meta_deprecated,
			const float p_meta_version,
			const String &p_meta_created,
			const String &p_meta_modified,
			const String &p_meta_collection) :

			meta_deprecated(p_meta_deprecated),
			meta_version(p_meta_version),
			meta_created(p_meta_created),
			meta_modified(p_meta_modified),
			meta_collection(p_meta_collection)

	{}

    float meta_deprecated;
    float meta_version;
    String meta_created;
    String meta_modified;
    String meta_collection;
};

struct GodoukenDataType 
{
	GodoukenDataType(
			const bool p_type_is_godot,
			const String &p_type_name) :

			type_is_godot(p_type_is_godot),
			type_name(p_type_name)

	{}

    bool type_is_godot;
    String type_name;
};


struct GodoukenDataParameter
{
	GodoukenDataParameter(
			const String &p_param_identifier,
			const String &p_param_desc,
			const GodoukenDataType &p_param_type_data) :

			param_identifier(p_param_identifier),
			param_desc(p_param_desc),
			param_type_data(p_param_type_data)

	{}

    String param_identifier;
    String param_desc;
    GodoukenDataType param_type_data;
};

struct GodoukenDataNodeSimple
{
	GodoukenDataNodeSimple(
			const String &p_node_name,
			const String &p_node_desc_brief,
			const String &p_node_desc_detailed) :

			node_name(p_node_name),
			node_desc_brief(p_node_desc_brief),
			node_desc_detailed(p_node_desc_detailed) {}

    String node_name;
    String node_desc_brief;
    String node_desc_detailed;
};

struct GodoukenDataNodeSignal : GodoukenDataNodeSimple
{
	GodoukenDataNodeSignal(
			const Vector<GodoukenDataParameter> p_signal_params,
			const GodoukenDataNodeSimple &p_node_data_simple) :

			GodoukenDataNodeSimple(p_node_data_simple),
			signal_params(p_signal_params)
			
	{}

    Vector<GodoukenDataParameter> signal_params;
};

struct GodoukenDataNodeBase : GodoukenDataNodeSimple
{
	GodoukenDataNodeBase(
			const String &p_node_extra_warning,
			const String &p_node_extra_info,
			const String &p_node_collection,
			const GodoukenDataType &p_node_return_type,
			const GodoukenDataNodeSimple &p_node_data_simple) :

			GodoukenDataNodeSimple(p_node_data_simple),
			node_extra_warning(p_node_extra_warning),
			node_extra_info(p_node_extra_info),
			node_collection(p_node_collection),
			node_return_type(p_node_return_type)

	{}

	GodoukenDataNodeBase(
			const GodoukenDataNodeBase &p_data_node_base,
			const GodoukenDataNodeSimple &p_data_node_simple) :

			GodoukenDataNodeSimple(p_data_node_simple),
			node_extra_warning(p_data_node_base.node_extra_warning),
			node_extra_info(p_data_node_base.node_extra_info),
			node_collection(p_data_node_base.node_collection),
			node_return_type(p_data_node_base.node_return_type)

	{}

    String node_extra_warning;
    String node_extra_info;
    String node_collection;
    GodoukenDataType node_return_type;
};

struct GodoukenDataNodeScript : GodoukenDataNodeBase
{
	GodoukenDataNodeScript(
			const String &p_script_timestamp_created,
			const String &p_script_timestamp_updated,
			const String &p_script_example,
			const Vector<String> p_script_authors,
			const Vector<String> p_script_see_also,
			const GodoukenDataNodeSimple &p_node_data_simple,
			const GodoukenDataNodeBase &p_node_data_base) :

			GodoukenDataNodeBase(p_node_data_base, p_node_data_simple),
			script_timestamp_created(p_script_timestamp_created),
			script_timestamp_updated(p_script_timestamp_updated),
			script_example(p_script_example),
			script_authors(p_script_authors),
			script_see_also(p_script_see_also)

	{}

    String script_timestamp_created;
    String script_timestamp_updated;
    String script_example;
    Vector<String> script_authors;
    Vector<String> script_see_also;
};

struct GodoukenDataNodeMethod : GodoukenDataNodeBase
{
	GodoukenDataNodeMethod(
			const bool p_method_is_godot,
			const bool p_method_is_internal,
			const bool p_method_is_overrider,
			const Vector<String> p_method_see_also,
			const Vector<GodoukenDataParameter> p_method_params,
			const Vector<GodoukenDataMetaLink> p_method_links,
			const Vector<GodoukenDataMetaTodo> p_method_todos,
			const Vector<GodoukenDataMetaRoadmap> p_method_roadmaps,
			const GodoukenDataNodeBase &p_node_data_base,
			const GodoukenDataNodeSimple &p_node_data_simple) :

			GodoukenDataNodeBase(p_node_data_base, p_node_data_simple),
			method_is_godot(p_method_is_godot),
			method_is_internal(p_method_is_internal),
			method_is_overrider(p_method_is_overrider),
			method_see_also(p_method_see_also),
			method_links(p_method_links),
			method_todos(p_method_todos),
			method_roadmaps(p_method_roadmaps)

	{}

    bool method_is_godot;
    bool method_is_internal;
    bool method_is_overrider;
    Vector<String> method_see_also;
    Vector<GodoukenDataParameter> method_params;
    Vector<GodoukenDataMetaLink> method_links;
    Vector<GodoukenDataMetaTodo> method_todos;
    Vector<GodoukenDataMetaRoadmap> method_roadmaps;
};

struct GodoukenDataNodeProperty : GodoukenDataNodeBase
{
	GodoukenDataNodeProperty(
			const bool p_property_is_exported,
			const GodoukenDataNodeBase &p_node_data_base,
			const GodoukenDataNodeSimple &p_node_data_simple) :

			GodoukenDataNodeBase(p_node_data_base, p_node_data_simple),
			property_is_exported(p_property_is_exported)

	{}

    bool property_is_exported;
};

#endif // GODOUKEN_DATA_TYPES_H
