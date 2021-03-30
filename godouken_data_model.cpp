/* godouken_data_model.cpp */

#include "editor/editor_node.h"
#include "editor/editor_scale.h"
#include "core/os/file_access.h"
#include "core/class_db.h"

#include "third_party/inja.hpp"

#include "godouken_data_model.h"

#include <cwctype>

#include "godouken_editor_pane.h"
#include "godouken_script_translator.h"
#include "modules/regex/regex.h"

#include "stencils/godouken_stencil_class_sidebar.h"
#include "stencils/godouken_stencil_class.h"
#include "stencils/godouken_stencil_directory.h"
#include "stencils/godouken_stencil_index.h"
#include "stencils/godouken_stencil_reference.h"
#include "stencils/godouken_stencil_style.h"

const String &GodoukenDataModel::extract_breadcrumb_html(const String &p_directory_path) {
	return p_directory_path.replace_first("res://", "").replace("/", "_") + ".html";
}

Vector<String> GodoukenDataModel::extract_breadcrumbs(const String &p_directory_path) {
	return p_directory_path.replace_first("res://", "").split("/");
}

void GodoukenDataModel::extract_breadcrumbs(const String &p_directory_path, nlohmann::json &p_breadcrumbs_json) {
	p_breadcrumbs_json["breadcrumbs"] = nlohmann::json::array();
	const Vector<String> breadcrumbs = extract_breadcrumbs((p_directory_path));
	for (uint32_t i = 0; i < breadcrumbs.size(); i++) {
		StringBuilder breadcrumb_url = StringBuilder();
		for (uint32_t x = 0; x < i; x--) {
			breadcrumb_url.append(breadcrumbs[x] + "_");
		}

		breadcrumb_url.append(breadcrumbs[i]);
		breadcrumb_url.append(".html");
		
		nlohmann::json breadcrumb_info = nlohmann::json::object();
		breadcrumb_info["name"] = breadcrumbs[i].utf8();
		breadcrumb_info["url"] = breadcrumb_url.as_string().utf8();
		p_breadcrumbs_json["breadcrumbs"].push_back(breadcrumb_info);
	}
}

bool GodoukenDataModel::contains(const PoolStringArray &p_array, const String &p_element) {
	bool found = false;
	for (int32_t i = 0; i < p_array.size(); i++) {
		if (p_array[i].find(p_element) >= 0) {
			found = true;
			break;
		}
	}

	return found;
}


////////////////////////////////////////////////////////////////////////////////////////////
////////
////       EVALUATION METHODS
////////
////////////////////////////////////////////////////////////////////////////////////////////

void GodoukenDataModel::evaluate_directories(GodoukenDirEntry *p_root_dir, const PoolStringArray &p_excluded) const {
	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Indexing directories");
	Vector<String> directories;
	directories.push_back("res://");
	DirAccess *dir = DirAccess::create(DirAccess::AccessType::ACCESS_RESOURCES);
	while (!directories.empty()) {
		dir->change_dir(directories[0]);
		dir->list_dir_begin();
		directories.remove(0);

		String dir_filename = "";
		while (!(dir_filename = dir->get_next()).empty()) {
			if (dir_filename[0] != '.') {
				const String &str_dir = dir->get_current_dir_without_drive();
				const String &str_path = (str_dir + "/" + dir_filename).replace("///", "//");
				if (dir->current_is_dir() && !contains(p_excluded, str_path)) {
					directories.push_back(str_path);
					tree_indexer(p_root_dir, extract_breadcrumbs(str_path));
				}
			}
		}
	}

	memdelete(dir);
	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Indexing directories ... done");
}

void GodoukenDataModel::evaluate_files(GodoukenDirEntry *p_root_dir, const PoolStringArray &p_excluded) {
	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Indexing files");
	Vector<String> directories;
	DirAccess *dir = DirAccess::create(DirAccess::AccessType::ACCESS_RESOURCES);
	directories.push_back("res://");
	while (!directories.empty()) {
		dir->change_dir(directories[0]);
		dir->list_dir_begin();
		directories.remove(0);

		String dir_filename = "";
		while (!(dir_filename = dir->get_next()).empty()) {
			if (dir_filename[0] != '.') {
				const String &str_dir = dir->get_current_dir_without_drive();
				const String &str_path = (str_dir + "/" + dir_filename).replace("///", "//");
				if (!contains(p_excluded, str_path)) {
					if (dir->current_is_dir()) {
						directories.push_back(str_path);
					}
					else {
						if (dir_filename.find(".gd") >= 0) {
							GodoukenDataEntry *directory_script = new GodoukenDataEntry();
							directory_script->data_directory = dir->get_current_dir_without_drive() + "/";
							directory_script->data_file = dir_filename;
							directory_script->data_name = dir_filename.substr(0, dir_filename.find_char('.'));
							model_data.insert(directory_script->data_name, directory_script);
							tree_indexer(p_root_dir, extract_breadcrumbs(str_dir), directory_script);
						}
					}
				}
			}
		}
	}

	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Indexing files ... done");
}

void GodoukenDataModel::evaluate_scripts() {
	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Evaluating Scripts");
	for (Map<String, GodoukenDataEntry*>::Element *E = model_data.front(); E; E = E->next()) {
		GodoukenTranslator *godouken_translator = memnew(GodoukenTranslator);
		GodoukenDataEntry *directory_script = E->value();
		GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Evaluating Scripts !" + directory_script->data_name);
		model_data.insert(directory_script->data_name, directory_script);
		godouken_translator->evaluate(directory_script->data_json, directory_script->data_file, directory_script->data_directory);
		directory_script->data_json["data"]["script"]["name_html"] = directory_script->data_name.utf8();

		RegEx expr("\[^a-zA-Z0-9]");
		const String &script_name_dt_dirty = directory_script->data_json["data"]["script"]["name_html"].get<std::string>().c_str();
		const String &script_name_dt = expr.sub(script_name_dt_dirty, " ", true).capitalize().replace(" ", "");
		directory_script->data_json["data"]["script"]["name_clean"] = script_name_dt.utf8();
		if (!model_graph.has(script_name_dt)) {
			model_graph.insert(script_name_dt, new GodoukenGraphEntry());
		}

		model_graph[script_name_dt]->graph_data = directory_script;
		const String &base_name_dirty = directory_script->data_json["data"]["script"]["base"]["name"].get<std::string>().c_str();
		const String &base_name = expr.sub(base_name_dirty, " ", true).capitalize().replace(" ", "");
		if (!base_name.empty()) {
			model_graph[script_name_dt]->graph_superclass = base_name;
			if (!model_graph.has(base_name)) {
				model_graph.insert(base_name, new GodoukenGraphEntry());
			}

			if (!model_graph[base_name]->graph_subclasses.find(script_name_dt)) {
				model_graph[base_name]->graph_subclasses.push_back(script_name_dt);
			}
		}

		memdelete(godouken_translator);
		GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Evaluating Scripts !" + directory_script->data_name + " ... done");
	}

	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Evaluating Scripts ... done");
}


////////////////////////////////////////////////////////////////////////////////////////////
////////
////       CREATION METHODS
////////
////////////////////////////////////////////////////////////////////////////////////////////

void GodoukenDataModel::create_directory_listings(GodoukenDirEntry *p_root_dir) const {
	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Generating directory listings");
	Vector<nlohmann::json> directories_json;
	Vector<GodoukenDirEntry *> dir_evaluation;
	Map<String, nlohmann::json> dir_alpha_global;
	dir_evaluation.push_back(p_root_dir);
	while (!dir_evaluation.empty()) {
		GodoukenDirEntry *directory_info = dir_evaluation[0];
		nlohmann::json directory_json = nlohmann::json::object();
		directory_json["entries"]["dirs"] = nlohmann::json::array();
		directory_json["entries"]["abc"] = nlohmann::json::array();
		directory_json["breadcrumbs"] = nlohmann::json::array();
		directory_json["url"] = tree_breadcrumb_html(directory_info).utf8();
		for (uint32_t i = 0; i < directory_info->dir_children.size(); i++) {
			const GodoukenDirEntry *dir_child = directory_info->dir_children[i];
			nlohmann::json directory_child = nlohmann::json::object();
			directory_child["name"] = dir_child->dir_name.utf8();
			directory_child["url"] = tree_breadcrumb_html(dir_child).utf8();
			directory_json["entries"]["dirs"].push_back(directory_child);
		}

		Map<String, nlohmann::json> directory_alpha_jsons;
		for (uint32_t i = 0; i < directory_info->dir_files.size(); i++) {
			GodoukenDataEntry *dir_data_entry = directory_info->dir_files[i];
			String alpha = dir_data_entry->data_name.substr(0, 1).to_lower();
			if (!directory_alpha_jsons.has(alpha)) {
				directory_alpha_jsons.insert(alpha, nlohmann::json::array());
			}

			if (!dir_alpha_global.has(alpha)) {
				dir_alpha_global.insert(alpha, nlohmann::json::array());
			}

			nlohmann::json directory_child = nlohmann::json::object();
			directory_child["name_script"] = dir_data_entry->data_file.utf8();
			directory_child["name"] = dir_data_entry->data_name.replace("_", " ").capitalize().utf8();
			directory_child["name_html"] = dir_data_entry->data_name.utf8();
			directory_child["path"] = dir_data_entry->data_directory.utf8();
			directory_alpha_jsons[alpha].push_back(directory_child);
			dir_alpha_global[alpha].push_back(directory_child);
		}

		for (Map<String, nlohmann::json>::Element *E = directory_alpha_jsons.front(); E; E = E->next()) {
			nlohmann::json dir_file_entry = nlohmann::json::object();
			dir_file_entry["alpha"] = E->key().to_upper().utf8();
			dir_file_entry["files"] = E->value();
			directory_json["entries"]["abc"].push_back(dir_file_entry);
		}

		for (uint32_t i = 0; i < directory_info->dir_children.size(); i++) {
			dir_evaluation.push_back(directory_info->dir_children[i]);
		}

		String project_title = ProjectSettings::get_singleton()->get_setting("application/config/name").operator String();
		directory_json["project"]["title"] = project_title.capitalize().utf8();

		tree_breadcrumb_json(directory_info, directory_json);
		directories_json.push_back(directory_json);
		dir_evaluation.remove(0);
	}

	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Generating directory listings ... done");
	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Generating Project Index Page");
	nlohmann::json index_json = nlohmann::json::object();
	index_json["entries"]["abc"] = nlohmann::json::array();
	index_json["project"]["title"] = ProjectSettings::get_singleton()->get_setting("application/config/name").operator String().capitalize().utf8();
	for (Map<String, nlohmann::json>::Element *E = dir_alpha_global.front(); E; E = E->next()) {
		nlohmann::json entry_json = nlohmann::json();
		entry_json["alpha"] = E->key().to_upper().utf8();
		entry_json["files"] = E->value();
		index_json["entries"]["abc"].push_back(entry_json);
	}

	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Generating Project Index Page ... collated");
	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Generating Project Index Page ... exporting");
	create_file(path_html, "index.html", godouken_stencil_index, index_json);
	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Generating Project Index Page ... done");
	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Generating Directory Pages");
	for (uint32_t i = 0; i < directories_json.size(); i++) {
		const String &name = directories_json[i]["url"].get<std::string>().c_str();
		GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Generating Directory Pages :: " + name);
		create_file(path_html_dirs, name, godouken_stencil_directory, directories_json[i]);
		GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Generating Directory Pages !" + name + " ... done");
	}
	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Generating Directory Pages ... done");
}

void GodoukenDataModel::create_inheritance_trees() {
	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Generating Inheritance Trees");
	for (Map<String, GodoukenDataEntry*>::Element *E = model_data.front(); E; E = E->next()) {
		GodoukenDataEntry *directory_script = E->value();
		GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Generating Inheritance Trees !" + directory_script->data_name);
		const String &script_name_dt = directory_script->data_json["data"]["script"]["name_clean"].get<std::string>().c_str();
		const GodoukenGraphEntry *entry_current = model_graph[script_name_dt];
		const GodoukenGraphEntry *entry_superclass = model_graph[entry_current->graph_superclass];
		const String &script_style_normal = ":::styleN";
		const String &script_style_super = entry_superclass && !entry_superclass->graph_data ? ":::styleG" : script_style_normal;

		StringBuilder script_graph_str;
		script_graph_str.append("\ngraph LR");
		script_graph_str.append("\n\t" + entry_current->graph_superclass + "[" + entry_current->graph_superclass + "]" + script_style_super);
		script_graph_str.append(" --> ");
		script_graph_str.append(script_name_dt + ":::styleC;");

		const String &class_html = "./" + entry_current->graph_data->data_name.replace(" ", "_") + ".html";
		script_graph_str.append("\n\tclick " + script_name_dt);
		script_graph_str.append(" href \"" + class_html + "\";");

		if (entry_superclass) {
			String subclass_html = "";
			if (entry_superclass->graph_data) {
				subclass_html = "\"./" + entry_superclass->graph_data->data_name.replace(" ", "_") + ".html\"";
			}
			else {
				subclass_html = "\"https://docs.godotengine.org/en/stable/classes/class_" + entry_current->graph_superclass.to_lower() + ".html\" _blank";
			}

			script_graph_str.append("\n\tclick " + entry_current->graph_superclass);
			script_graph_str.append(" href " + subclass_html + ";");
		}

		for (uint32_t x = 0; x < entry_current->graph_subclasses.size(); x++) {
			const String &graph_subclass = entry_current->graph_subclasses[x];
			const String &graph_html = "./" + model_graph[graph_subclass]->graph_data->data_name.replace(" ", "_") + ".html";
			script_graph_str.append("\n\t" + script_name_dt);
			script_graph_str.append(" --> ");
			script_graph_str.append(graph_subclass + script_style_normal + ";");
			script_graph_str.append("\n\tclick " + graph_subclass);
			script_graph_str.append(" href \"" + graph_html + "\";");
		}

		script_graph_str.append("\n\tclassDef styleG fill:#478CBF,stroke:#111,stroke-width:3px,font-size:0.96em,color:#FFF;");
		script_graph_str.append("\n\tclassDef styleC fill:#FF9933,stroke:#111,stroke-width:3px,font-size:0.96em,color:#FFF;");
		script_graph_str.append("\n\tclassDef styleN fill:#FFFFFF,stroke:#111,stroke-width:3px,font-size:0.96em;\n");
		directory_script->data_json["data"]["script"]["diagram"] = script_graph_str.as_string().utf8();
		GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Generating Inheritance Trees !" + directory_script->data_name + " ... done");
	}

	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Generating Inheritance Trees ... done");
}

void GodoukenDataModel::create_script_pages() const {
	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Generating Scripts");
	nlohmann::json sidebar_json = nlohmann::json::object();
	sidebar_json["data"]["scripts"] = nlohmann::json::array();
	sidebar_json["data"]["reference"] = ProjectSettings::get_singleton()->get_setting("godouken/config/include_reference").operator bool();
	for (Map<String, GodoukenDataEntry*>::Element *E = model_data.front(); E; E = E->next()) {
		GodoukenDataEntry *script_entry = E->value();
		GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Generating Scripts !" + script_entry->data_name);
		script_entry->data_json["data"]["project"]["title"] = ProjectSettings::get_singleton()->get_setting("application/config/name").operator String().capitalize().utf8();
		script_entry->data_json["data"]["script"]["breadcrumbs"] = nlohmann::json::array();
		if (!script_entry->data_json["data"]["script"]["name_sm"].empty()) {
			tree_breadcrumb_json(script_entry->data_parent_dir, script_entry->data_json["data"]["script"]);
			nlohmann::json breadcrumb_script;
			String name_html = script_entry->data_json["data"]["script"]["name_html"].get<std::string>().c_str();
			breadcrumb_script["name"] = (name_html + ".gd").utf8();
			breadcrumb_script["url"] = "";
			script_entry->data_json["data"]["script"]["breadcrumbs"].push_back(breadcrumb_script);
			script_entry->data_json["data"]["script"]["breadcrumbs"][0]["url"] = "..html";

			nlohmann::json sidebar_entry;
			sidebar_entry["name_clean"] = script_entry->data_json["data"]["script"]["name_clean"];
			sidebar_entry["name_html"] = script_entry->data_json["data"]["script"]["name_html"];
			sidebar_json["data"]["scripts"].push_back(sidebar_entry);
		}

		const String &name = script_entry->data_name.replace(" ", "_") + ".html";
		create_file(path_html, name, godouken_stencil_class, script_entry->data_json);
		GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Generating Scripts !" + script_entry->data_name + " ... done");
	}

	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Generating Scripts ... done");
	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Exporting Sidebar");
	create_file(path_html_generic, "sidebar.html", godouken_stencil_class_sidebar, sidebar_json);
	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Exporting Sidebar ... done");
}

void GodoukenDataModel::create_reference_page() const {
	const bool use_reference = ProjectSettings::get_singleton()->get_setting("godouken/config/include_reference").operator bool();
	if (use_reference) {
		GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Exporting Reference Manual");
		nlohmann::json ref_json = nlohmann::json::object();
		ref_json["project"]["title"] = ProjectSettings::get_singleton()->get_setting("application/config/name").operator String().capitalize().utf8();
		create_file(path_html, "reference.html", godouken_stencil_reference, ref_json);
		GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Exporting Reference Manual ... done");
	}
}

void GodoukenDataModel::create_styles() const {
	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Exporting Styles");
	create_file(path_css, "main.css", godouken_stencil_style, nullptr);
	GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Exporting Styles ... done");
}

void GodoukenDataModel::create_file(const String &p_dir, const String &p_name, const std::string &p_template, const nlohmann::json &p_template_json) {
	inja::Environment environment;
	const std::string result = environment.render(p_template, p_template_json);
	DirAccess *dir = DirAccess::create(DirAccess::AccessType::ACCESS_RESOURCES);
	dir->change_dir(p_dir);
	if (!dir->dir_exists(p_dir)) {
		dir->make_dir_recursive(p_dir);
	}

	FileAccess *file = FileAccess::create(FileAccess::AccessType::ACCESS_RESOURCES);
	file->reopen(p_dir + p_name, FileAccess::WRITE);
	file->store_string(result.c_str());
	file->close();
	memdelete(file);
	memdelete(dir);
}


////////////////////////////////////////////////////////////////////////////////////////////
////////
////       SPECIAL PUBLIC GENERATION METHODS (RELIES ON CREATION / EVALUATION METHODS)
////////
////////////////////////////////////////////////////////////////////////////////////////////

void GodoukenDataModel::generate() {
	if (!model_compiling) {
		const uint64_t time_start = OS::get_singleton()->get_system_time_msecs();
		model_compiling = true;
		path = ProjectSettings::get_singleton()->get_setting("godouken/config/docs_path").operator String();
		path_css = path + "/css/";
		path_html = path + "/html/";
		path_html_dirs = path_html + "dirs/";
		path_html_generic = path_html + "generic/";

		const PoolStringArray excluded = ProjectSettings::get_singleton()->get_setting("godouken/config/exclude_directories").operator PoolVector<String>();
		GodoukenEditorPane::get_singleton()->log_flush();
		GodoukenDirEntry *root_dir = new GodoukenDirEntry();
		root_dir->dir_name = ".";

		model_graph.clear();
		model_data.clear();
		
		evaluate_directories(root_dir, excluded);
		evaluate_files(root_dir, excluded/*, directories_scripts*/);
		create_directory_listings(root_dir);
		evaluate_scripts(/*directories_scripts*/);
		create_inheritance_trees(/*directories_scripts*/);
		create_script_pages();
		create_reference_page();
		create_styles();
		
		const uint64_t time_end = OS::get_singleton()->get_system_time_msecs();
		const uint64_t time_elapsed = time_end - time_start;
		const float time_secs = static_cast<float>(time_elapsed) / 1000.0f;
		const std::string time_c = std::to_string(time_secs);
		StringBuilder time_str;
		time_str.append("Compilation took ");
		time_str.append(time_c.c_str());
		time_str.append("s");

		GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - Exporting Sidebar ... done");
		GodoukenEditorPane::get_singleton()->log_insert("[Compiling] - SUCCESSFUL | " + time_str.as_string());
		GodoukenEditorPane::get_singleton()->log_insert("--");
		GodoukenEditorPane::get_singleton()->log_insert("");
		model_compiling = false;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////
////////
////       TREE INDEXING HELPER METHODS
////////
////////////////////////////////////////////////////////////////////////////////////////////

void GodoukenDataModel::tree_indexer(GodoukenDirEntry *p_node, Vector<String> p_breadcrumbs) {
	GodoukenDirEntry *dir_entry = p_node;
	while (!p_breadcrumbs.empty()) {
		if (!p_breadcrumbs[0].empty()) {
			GodoukenDirEntry *dir_found_entry = dir_entry;
			bool dir_is_found = false;
			for (uint32_t i = 0; i < dir_entry->dir_children.size(); i++) {
				if (p_breadcrumbs[0].find(dir_entry->dir_children[i]->dir_name) >= 0) {
					dir_is_found = true;
					dir_found_entry = dir_entry->dir_children[i];
					break;
				}
			}

			if (!dir_is_found) {
				GodoukenDirEntry *dir_child = new GodoukenDirEntry();
				dir_child->dir_name = p_breadcrumbs[0];
				dir_child->dir_parent = dir_entry;
				dir_entry->dir_children.push_back(dir_child);
				dir_entry = dir_child;
			}
			else {
				dir_entry = dir_found_entry;
			}
		}
		
		p_breadcrumbs.remove(0);
	}
}

void GodoukenDataModel::tree_indexer(GodoukenDirEntry *p_node, Vector<String> p_breadcrumbs, GodoukenDataEntry *p_data_entry) {
	bool success;
	tree_indexer(p_node, p_breadcrumbs, p_data_entry, success);
}

void GodoukenDataModel::tree_indexer(GodoukenDirEntry *p_node, Vector<String> p_breadcrumbs, GodoukenDataEntry *p_data_entry, bool &p_success) {
	p_success = false;
	GodoukenDirEntry *dir_entry = p_node;
	while (!p_breadcrumbs.empty()) {
		GodoukenDirEntry *dir_found_entry = dir_entry;
		bool dir_is_found = false;
		for (uint32_t i = 0; i < dir_entry->dir_children.size(); i++) {
			if (p_breadcrumbs[0].find(dir_entry->dir_children[i]->dir_name) >= 0) {
				dir_is_found = true;
				dir_found_entry = dir_entry->dir_children[i];
				break;
			}
		}

		if (dir_is_found) {
			dir_entry = dir_found_entry;
		}

		p_breadcrumbs.remove(0);
		if (p_breadcrumbs.empty()) {
			dir_entry->dir_files.push_back(p_data_entry);
			p_data_entry->data_parent_dir = dir_entry;
			p_success = true;
		}
	}
}

Vector<String> GodoukenDataModel::tree_breadcrumbs(GodoukenDirEntry *p_node) {
	Vector<String> breadcrumbs;
	GodoukenDirEntry *node_current = p_node;
	breadcrumbs.push_back(node_current->dir_name);
	while (node_current->dir_parent) {
		node_current = node_current->dir_parent;
		breadcrumbs.push_back(node_current->dir_name);
	}

	return breadcrumbs;
}

String GodoukenDataModel::tree_breadcrumb_html(const GodoukenDirEntry *p_node)
{
	String result = "";
	if (p_node) {
		const GodoukenDirEntry *node_current = p_node;
		String breadcrumb = ".html";
		String breadcrumb_end = "";
		bool breadcrumb_first = true;
		//breadcrumb = node_current->dir_name + breadcrumb;
		do {
			breadcrumb_end = breadcrumb_first ? "" : "_";
			breadcrumb = node_current->dir_name + breadcrumb_end + breadcrumb;
			breadcrumb_first = false;
			node_current = node_current->dir_parent;
		} while (node_current);

		result = breadcrumb;
	}
	
	return result;
}

String GodoukenDataModel::tree_breadcrumb_html(const Vector<String> &p_breadcrumbs) {
	StringBuilder breadcrumb_html = StringBuilder();
	String breadcrumb_end = "";
	for (uint32_t i = 1; i < p_breadcrumbs.size(); i++) {
		breadcrumb_end = i != p_breadcrumbs.size() - 1 ? "_" : "";
		breadcrumb_html.append(p_breadcrumbs[i] + breadcrumb_end);
	}

	breadcrumb_html.append(".html");
	return breadcrumb_html.as_string();
}

void GodoukenDataModel::tree_breadcrumb_json(const GodoukenDirEntry *p_node, nlohmann::json &p_breadcrumbs)
{
	if (p_node) {
		const GodoukenDirEntry *node_current = p_node;
		List<nlohmann::json> breadcrumbs_json;
		do {
			nlohmann::json breadcrumb = nlohmann::json::object();
			breadcrumb["name"] = node_current->dir_name.utf8();
			breadcrumb["url"] = tree_breadcrumb_html(node_current).utf8();
			breadcrumbs_json.push_back(breadcrumb);
			node_current = node_current->dir_parent;
		} while (node_current);

		breadcrumbs_json.invert();
		for (uint32_t i = 0; i < breadcrumbs_json.size(); i++) {
			p_breadcrumbs["breadcrumbs"].push_back(breadcrumbs_json[i]);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////
////////
////       CLASS CONSTRUCTOR / DESTRUCTOR
////////
////////////////////////////////////////////////////////////////////////////////////////////

GodoukenDataModel::GodoukenDataModel() :
	model_compiling(false)
{}

GodoukenDataModel::~GodoukenDataModel() {}
