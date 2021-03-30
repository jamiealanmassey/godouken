/* godouken_data_model.h */

#include "godouken_data_types.h"

#include "core/ustring.h"
#include "core/object.h"
#include "core/map.h"
#include "third_party/json.hpp"

#ifndef GODOUKEN_DATA_MODEL_H
#define GODOUKEN_DATA_MODEL_H

static Vector<String> extract_breadcrumbs(const String &p_directory_path);

struct GodoukenDirEntry;
struct GodoukenDataEntry {
	String data_name;
	String data_file;
	String data_directory;
	nlohmann::json data_json;
	GodoukenDirEntry *data_parent_dir;
};

struct GodoukenDirEntry {
	String dir_name;
	Vector<GodoukenDataEntry *> dir_files;
	Vector<GodoukenDirEntry *> dir_children;
	GodoukenDirEntry *dir_parent;

	GodoukenDirEntry() {
		dir_name = "";
		dir_parent = nullptr;
	}
	
	GodoukenDirEntry(GodoukenDirEntry &p_entry) {
		this->dir_name = p_entry.dir_name;
		this->dir_files = p_entry.dir_files;
		this->dir_children = p_entry.dir_children;
		this->dir_parent = p_entry.dir_parent;
	}
};

struct GodoukenGraphEntry {
	String graph_superclass;
	List<String> graph_subclasses;
	GodoukenDataEntry *graph_data;

	GodoukenGraphEntry() {
		graph_data = nullptr;
	}
};

class GodoukenDataModel : public Object {
    GDCLASS(GodoukenDataModel, Object);

protected:
	bool model_compiling;
	
	Map<String, GodoukenDataEntry *> model_data;
	Map<StringName, GodoukenGraphEntry *> model_graph;
	Vector<GodoukenDataEntry *> model_directory_scripts;
	
	String path;
	String path_css;
	String path_html;
	String path_html_dirs;
	String path_html_generic;
	
private:
	static const String &extract_breadcrumb_html(const String &p_directory_path);
	static Vector<String> extract_breadcrumbs(const String &p_directory_path);
	static void extract_breadcrumbs(const String &p_directory_path, nlohmann::json &p_breadcrumbs_json);
	static bool contains(const PoolStringArray &p_array, const String &p_element);
	
	void evaluate_directories(GodoukenDirEntry *p_root_dir, const PoolStringArray &p_excluded) const;
	void evaluate_files(GodoukenDirEntry *p_root_dir, const PoolStringArray &p_excluded);
	void evaluate_scripts();
	
	void create_directory_listings(GodoukenDirEntry *p_root_dir) const;
	void create_inheritance_trees();
	
	void create_script_pages() const;
	void create_reference_page() const;
	void create_styles() const;
	
	static void create_file(const String &p_dir, const String &p_name, const std::string &p_template, const nlohmann::json &p_template_json);
	
public:
	void generate();

protected:
	static void tree_indexer(GodoukenDirEntry *p_node, Vector<String> p_breadcrumbs);
	static void tree_indexer(GodoukenDirEntry *p_node, Vector<String> p_breadcrumbs, GodoukenDataEntry *p_data_entry);
	static void tree_indexer(GodoukenDirEntry *p_node, Vector<String> p_breadcrumbs, GodoukenDataEntry *p_data_entry, bool &p_success);

	static Vector<String> tree_breadcrumbs(GodoukenDirEntry *p_node);
	static String tree_breadcrumb_html(const GodoukenDirEntry *p_node);
	static String tree_breadcrumb_html(const Vector<String> &p_breadcrumbs);
	static void tree_breadcrumb_json(const GodoukenDirEntry *p_node, nlohmann::json &p_breadcrumbs);
	
public:
    GodoukenDataModel();
    ~GodoukenDataModel();
};


#endif // GODOUKEN_DATA_MODEL_H
