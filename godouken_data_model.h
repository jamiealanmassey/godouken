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
    uint32_t model_script_hash;
    GodoukenDataNodeBase *model_node_root;

public:
	void data();

	Map<String, GodoukenDataEntry *> godouken_model;

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
